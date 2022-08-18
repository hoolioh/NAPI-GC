const NapiMixV8 = require("../lib/binding.js");
const assert = require("assert");

assert(NapiMixV8, "The expected function is undefined");

function testBasic()
{
    const result =  NapiMixV8("hello");
    assert.strictEqual(result, "world", "Unexpected value returned");
}

assert.doesNotThrow(testBasic, undefined, "testBasic threw an expection");

console.log("Tests passed- everything looks OK!");