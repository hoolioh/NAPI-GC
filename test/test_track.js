const { Track, Check, SetGCCallback } = require("../lib/binding.js");
const assert = require("assert");

assert(Track, "The expected function is undefined");
assert(Check, "The expected function is undefined");
assert(SetGCCallback, "The expected function is undefined");

function doSweep() {
    if (global.gc) {
        global.gc();
    }
}

let ret;
let obj = { x: 1, y: 2 };

ret = Check()
assert.strictEqual(ret, false, "Unexpected value");

Track(obj);
ret = Check();
assert.strictEqual(ret, true, "Unexpected value");

obj.z = 3 
doSweep();
ret = Check();
assert.strictEqual(ret, true, "Unexpected value");

obj = {};
doSweep();
//Checking ref to previous object, it should return udefined unless it was reset.
ret = Check();
assert.strictEqual(ret, undefined, "Unexpected value");

//Enabling reference collector
SetGCCallback();
obj = null;
doSweep();
//Checking ref to empty object this time, since GC callbacks are enabled, it 
//should return false
ret = Check();
assert.strictEqual(ret, false, "Unexpected value");

console.log("Tests passed");
