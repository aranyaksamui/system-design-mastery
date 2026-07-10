// Simulating a network request that takes 2 seconds
function handleRequestNonBlocking(requestId) {
    console.log(`[Main Thread] Request ${requestId} received. Offloading I/O to kernel...`);

    // The timer simulates an asynchronous OS system call
    // The Main Thread does not stop here
    setTimeout(() => {
        console.log(`[Event Loop] I/O for Request ${requestId} finished. Executing callback.`);
    }, 2000)
}

console.log("Starting single-threaded server simulation...");

// Simulate 3 network request
for (let i = 0; i < 3; i++)
    handleRequestNonBlocking(i);

console.log("[Main Thread] I am totally free to do other work until the network operations are on going");
console.log("Main call stack is now empty");
    

