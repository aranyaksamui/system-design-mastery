## Part 1: Deep Dive into Middlewares

A middleware is simply a function that has access to the **`req`** object, the **`res`** object, and the **`next`** function.

```typescript
function myMiddleware(req, res, next) {
  // 1. Inspect or modify req / res
  // 2. Terminate the request by sending a response (e.g., res.status(401).json(...))
  // 3. OR pass control to the next middleware in line:
  next();
}
```

> **Crucial Rule:** Every middleware *must* either call `next()` to pass control to the next handler, or return a response (e.g., `res.json()`). If it does neither, the request **hangs forever** until the client times out.

---

### The 4 Major Categories of Middleware

```
Incoming Request
      │
      ▼
┌────────────────────────────────────────────────────────┐
│ 1. Built-in & Parsing Middleware (express.json, etc.)  │
└────────────────────────┬───────────────────────────────┘
                         │ next()
                         ▼
┌────────────────────────────────────────────────────────┐
│ 2. Third-Party / Cross-Cutting (CORS, Helmet, Morgan) │
└────────────────────────┬───────────────────────────────┘
                         │ next()
                         ▼
┌────────────────────────────────────────────────────────┐
│ 3. Router / Application Middleware (Auth, Validation)  │
└────────────────────────┬───────────────────────────────┘
                         │
        ┌────────────────┴────────────────┐
   (No Error)                          (Error: next(err))
        ▼                                 ▼
┌──────────────────────┐         ┌───────────────────────────────┐
│ Send Normal Response │         │ 4. Error-Handling Middleware  │
│ (200, 201, etc.)     │         │ ((err, req, res, next))       │
└──────────────────────┘         └───────────────────────────────┘
```

#### 1. Built-in & Body-Parsing Middleware

Node.js receives raw HTTP data in binary streams/chunks. These middlewares assemble the incoming chunks and deserialize them onto `req.body`.

* `express.json()`: Parses incoming payloads with JSON content types (`application/json`).
* `express.urlencoded({ extended: true })`: Parses URL-encoded bodies (often used with standard HTML `<form>` submissions).
* `express.static('public')`: Serves static files (images, CSS, HTML files).

#### 2. Cross-Cutting & Third-Party Middleware

Utility middlewares that execute across most or all routes to enforce security, networking, or observability rules.

* **`cors()`**: Sets headers like `Access-Control-Allow-Origin` to allow or restrict frontend domains.
* **`helmet()`**: Sets security-focused HTTP headers (e.g., Content Security Policy, X-Frame-Options to prevent clickjacking).
* **`morgan()` / `pino-http`**: Request loggers that log incoming HTTP verbs, paths, response codes, and execution times.
* **`express-rate-limit`**: Prevents brute-force or DDoS attacks by limiting requests per IP within a time window.

#### 3. Router & Application-Level Middleware (Guards & Validators)

Custom business logic applied to specific routes or route groups.

* **Authentication Guard**: Verifies JWTs or session cookies and attaches the authenticated user to `req.user`.
* **Authorization / RBAC**: Checks if `req.user.role === 'ADMIN'`. If not, returns `403 Forbidden`.
* **Schema Validation (Zod/Joi)**: Validates `req.body` or `req.query` against a schema before hitting the database.

#### 4. Error-Handling Middleware

Express recognizes error handlers by their **exact 4-argument signature**: `(err, req, res, next)`.

* Placed at the very end of the middleware stack (after all routes).
* Triggered whenever any middleware or controller passes an argument into `next(error)` or throws an unhandled error.
* Centralizes error responses, prevents stack trace leaks to the client in production, and standardizes status codes:

```typescript
app.use((err, req, res, next) => {
  console.error(err.stack);
  const status = err.statusCode || 500;
  res.status(status).json({
    success: false,
    message: err.message || 'Internal Server Error',
    ...(process.env.NODE_ENV === 'development' && { stack: err.stack })
  });
});
```

## Part 2: The `req` (Request) Object Model

The `req` object represents the incoming HTTP request. It inherits from Node's `http.IncomingMessage` and is decorated with Express properties and custom middleware properties.

```
┌─────────────────────────────────────────────────────────────┐
│                         req Object                          │
├──────────────────────────────┬──────────────────────────────┤
│ Standard Route Data          │ Network & Transport          │
│  • req.params (URL params)   │  • req.headers               │
│  • req.query  (query string) │  • req.ip / req.ips          │
│  • req.body   (parsed body)  │  • req.method / req.path     │
├──────────────────────────────┼──────────────────────────────┤
│ Security & State             │ Middleware-Injected          │
│  • req.cookies               │  • req.user (Auth/JWT)       │
│  • req.signedCookies         │  • req.session (Sessions)    │
│                              │  • req.file(s) (Multer)      │
└──────────────────────────────┴──────────────────────────────┘
```

### 1. Data Extractors (Where client data comes in)

* **`req.params`**: Extracts route path variables.
  * Example: Route `/users/:id` matching `/users/64f1a2b...` $\rightarrow$ `req.params.id`.
* **`req.query`**: Extracts query strings after the `?` in the URL.
  * Example: `/posts?page=2&limit=10` $\rightarrow$ `req.query.page`, `req.query.limit`.
* **`req.body`**: Holds the payload parsed by `express.json()`.
  * Example: `{ "name": "Alice", "email": "alice@example.com" }`.

### 2. Network & Headers

* **`req.headers`**: An object containing all incoming HTTP headers (e.g., `req.headers.authorization`, `req.headers['content-type']`).
* **`req.method`**: The HTTP verb (`'GET'`, `'POST'`, `'PATCH'`, etc.).
* **`req.path` vs `req.originalUrl`**:
  * `req.path`: The path portion of the URL (e.g., `/api/v1/users`).
  * `req.originalUrl`: The full URL including query parameters, preserved even across nested sub-routers.
* **`req.ip` / `req.ips`**: The remote IP address of the client (if behind a proxy like Nginx or Cloudflare, enable `app.set('trust proxy', true)` so `req.ip` reads `X-Forwarded-For`).

### 3. Middleware-Injected Properties (Custom Subparts)

By default, Express does not have these properties; **they are attached by middlewares** to pass state down the pipeline:

* **`req.user`** *(Attached by Auth Middleware)*:
  * When a JWT or API token is verified, the middleware decodes the token and attaches the user document/identity to `req.user`. Downstream controllers can access `req.user.id` or `req.user.role` without querying the database again.
* **`req.session`** *(Attached by `express-session`)*:
  * If using server-side session stores (like Redis or MongoDB), `req.session` stores and mutates session data (e.g., `req.session.userId = user.id`).
* **`req.cookies` & `req.signedCookies`** *(Attached by `cookie-parser`)*:
  * Contains parsed cookies sent in the `Cookie` HTTP header. Signed cookies verify that the client hasn't tampered with the cookie value.
* **`req.file` / `req.files`** *(Attached by `multer`)*:
  * For `multipart/form-data` file uploads, `multer` processes the binary stream and attaches file metadata (e.g., buffer, file size, mime type, S3 path) to `req.file`.
* **`req.id` / `req.correlationId`** *(Attached by tracing middleware)*:
  * A unique UUID per request used to trace a single request's path through logs and microservices.

## Part 3: The `res` (Response) Object Model

The `res` object represents the outgoing HTTP response that Express sends back to the client. It wraps Node’s native `http.ServerResponse`.

```
┌─────────────────────────────────────────────────────────────┐
│                         res Object                          │
├──────────────────────────────┬──────────────────────────────┤
│ Sending Data                 │ Cookies & Redirection        │
│  • res.status(code)          │  • res.cookie(name, val)     │
│  • res.json(data)            │  • res.clearCookie(name)     │
│  • res.send(body)            │  • res.redirect(url)         │
├──────────────────────────────┼──────────────────────────────┤
│ Headers & Internals          │ Request-Scoped Shared State  │
│  • res.setHeader()           │  • res.locals                │
│  • res.headersSent (boolean) │    (Data shared across       │
│  • res.end()                 │     middlewares & templates) │
└──────────────────────────────┴──────────────────────────────┘
```

### 1. Methods for Sending Responses

* **`res.status(code)`**: Sets the HTTP status code (e.g., `200`, `201`, `400`, `404`, `500`). It is chainable: `res.status(201).json(...)`.
* **`res.json(data)`**: Converts a JavaScript object/array to JSON, sets the `Content-Type: application/json` header, and terminates the response.
* **`res.send(body)`**: Generic sender. Can send strings, HTML buffers, or objects. (Prefer `res.json()` for APIs).
* **`res.sendStatus(code)`**: Sets the status code and sends its string representation as the response body (e.g., `res.sendStatus(404)` sends `"Not Found"`).
* **`res.redirect([status,] url)`**: Sends a 301/302 redirect header to send the client to a new URL.

### 2. Header and Cookie Controls

* **`res.setHeader(name, value)`**: Sets a single HTTP response header (e.g., `res.setHeader('Cache-Control', 'no-store')`).
* **`res.cookie(name, value, [options])`**: Sets an HTTP cookie on the client with security flags:

  ```typescript
  res.cookie('token', jwtToken, {
    httpOnly: true, // Inaccessible to client JS (prevents XSS reading the cookie)
    secure: process.env.NODE_ENV === 'production', // Transmitted only via HTTPS
    sameSite: 'strict', // CSRF protection
    maxAge: 24 * 60 * 60 * 1000 // 1 day
  });
  ```

* **`res.clearCookie(name)`**: Expire and remove a cookie from the client.

### 3. Critical Properties & Diagnostics

* **`res.locals` (Request-Scoped State)**:
  * An object scoped **strictly to the lifetime of the current request**.
  * Perfect for passing data between middlewares without polluting the `req` object.

  ```typescript
  // In an auth middleware:
  res.locals.startTime = Date.now();
  res.locals.user = authenticatedUser;
  ```

* **`res.headersSent` (Boolean)**:
  * Indicates whether HTTP headers have already been sent to the client.
  * **Why it matters:** If you attempt to send two responses for the same request (e.g., missing a `return` statement before `res.json()`), Express throws the infamous error:
    `"Error [ERR_HTTP_HEADERS_SENT]: Cannot set headers after they are sent to the client"`. Checking `if (res.headersSent) return;` prevents this in complex async flows.

## Part 4: How It All Connects in a Prisma Flow

Here is a practical flow showing how middlewares mutate `req`, pass data, handle errors, and return a clean `res`:

```typescript
import express from 'express';
import { prisma } from './prismaClient';

const app = express();

// 1. Built-in Parsing Middleware
app.use(express.json());

// 2. Custom Authentication Middleware (Injects req.user)
const requireAuth = async (req, res, next) => {
  try {
    const authHeader = req.headers.authorization;
    if (!authHeader?.startsWith('Bearer ')) {
      return res.status(401).json({ message: 'Unauthorized: No token provided' });
    }

    const token = authHeader.split(' ')[1];
    const decoded = verifyJwt(token); // Verification function

    // Attach to req object for downstream handlers
    req.user = decoded; 
    next(); // Pass to the next handler
  } catch (error) {
    next(error); // Route to global error handler
  }
};

// 3. Route & Controller (Uses req.user, req.params, req.body, and Prisma)
app.post('/api/posts/:categoryId', requireAuth, async (req, res, next) => {
  try {
    const { categoryId } = req.params;     // From URL param
    const { title, content } = req.body;    // From JSON body
    const authorId = req.user.id;           // Injected by requireAuth

    const post = await prisma.post.create({
      data: {
        title,
        content,
        authorId, // MongoDB ObjectId reference
        categoryId,
      },
    });

    // Send successful response via res
    return res.status(201).json({ success: true, data: post });
  } catch (err) {
    next(err); // Trigger 4-param error handler
  }
});

// 4. Centralized Error-Handling Middleware (Must be last)
app.use((err, req, res, next) => {
  res.status(err.status || 500).json({
    success: false,
    error: err.message || 'Internal Server Error',
  });
});
```
