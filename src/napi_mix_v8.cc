#include <iostream>
#include <napi.h>
#include <v8.h>
#include <map>
#include <vector>

using namespace Napi;

Napi::ObjectReference* _ref = nullptr;

#define ASSIGN_REF(ref,new)  do {                                       \
                                if (ref) {                              \
                                    ref->Reset();                       \
                                    delete ref;                         \
                                }                                       \
                                ref = new;                              \
                            } while(0);

#define CLEAN_REF(ref)  do {                                            \
                            if (ref && ref->Value().IsUndefined()) {    \
                                ref->Reset();                           \
                                delete ref;                             \
                                ref = nullptr;                          \
                            }                                           \
                        } while(0);


void collectReferences(v8::Isolate *isolate, v8::GCType type, v8::GCCallbackFlags flags) {
    std::cout << "Collecting references" << std::endl;
    switch (type) {
        case v8::GCType::kGCTypeScavenge:
            CLEAN_REF(_ref);
            break;
        case v8::GCType::kGCTypeMarkSweepCompact:
            CLEAN_REF(_ref);
            break;
        default:
            // Not a phase where objects are freed or moved to another generation.
            break;
    }
}

Napi::String Greet(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    //Greet
    return Napi::String::New(env, "Hello world!");
}

Napi::Value Track(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong number of args").ThrowAsJavaScriptException();
    }

    if (!info[0].IsFunction() && !info[0].IsObject()) {
        std::cout << "Not refereceable" << std::endl;
        return env.Null();
    }

    Napi::ObjectReference* ref = new Napi::ObjectReference();
    ref->Reset(info[0].As<Napi::Object>());
    ASSIGN_REF(_ref, ref);

    return env.Null();

}

Napi::Value Check(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    Napi::Reference<Napi::Object> *ref = _ref;

    if (!ref) {
        return Napi::Boolean::New(env, false);
    } else if (ref->IsEmpty()) {
        return env.Null();
    } else if (ref->Value().IsUndefined()) {
        return env.Undefined();
    } else {
        return Napi::Boolean::New(env, true);
    }

}

Napi::Value SetCallback(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    v8::Isolate *i = v8::Isolate::GetCurrent();

    i->AddGCEpilogueCallback(collectReferences, v8::GCType::kGCTypeAll);
    return Napi::Boolean::New(env, true);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "Greet"),
            Napi::Function::New(env, Greet));

    exports.Set(Napi::String::New(env, "Track"),
            Napi::Function::New(env, Track));

    exports.Set(Napi::String::New(env, "Check"),
            Napi::Function::New(env, Check));

    exports.Set(Napi::String::New(env, "SetGCCallback"),
            Napi::Function::New(env, SetCallback));

    return exports;
}

NODE_API_MODULE(addon, Init)
