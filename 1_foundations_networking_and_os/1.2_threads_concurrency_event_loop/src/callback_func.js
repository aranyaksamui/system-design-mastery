// The JavaScript filter functions accepts a callback function (memory pointer to a function)
// A callback function like this is just a custom behaviour that we write which then gets performed on each of the element of the array
let arr = [10, 15, 20, 25];
const result = arr.filter((num) => {
    return num > 18;
});

console.log(result);


// Writing a custom filter function to understand callback functions
const customFilter = (arr, callBackPointer) => {
    let resultArr = [];

    for (let i = 0; i < arr.length; i++) {
        const check = callBackPointer(arr[i]);
        if (check) resultArr.push(arr[i]);
    }

    return resultArr;
}

let arr1 = [10, 15, 18, 20, 25, 30]
const ans = customFilter(arr1, (n) => {
    return n > 18;
});

console.log(ans);
