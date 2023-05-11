#include "zfs_wrap.h"
#include "env-inl.h"
#include "util-inl.h"
#include "node_internals.h"
#include "node_external_reference.h"


namespace node {

using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;
using v8::Isolate;
using v8::MaybeLocal;

void Zfs::Initialize(Local<Object> target,
                         Local<Value> unused,
                         Local<Context> context,
                         void* priv) {
  Environment* env = Environment::GetCurrent(context);
  Isolate *isolate = env->isolate();
  
  // Local<FunctionTemplate> constructor = NewFunctionTemplate(isolate, New);

  // SetProtoMethod(isolate, constructor, "init", Init);

  // SetConstructorFunction(context, target, "Zfs", constructor);

  Local<FunctionTemplate> t = NewFunctionTemplate(isolate, Init);
  Local<String> str = FIXED_ONE_BYTE_STRING(env->isolate(), "init");
  t->SetClassName(str);

  target->Set(env->context(),
              str,
              t->GetFunction(env->context()).ToLocalChecked()).Check();

  t = NewFunctionTemplate(isolate, Read);
  str = FIXED_ONE_BYTE_STRING(env->isolate(), "read");
  t->SetClassName(str);

  target->Set(env->context(),
              str,
              t->GetFunction(env->context()).ToLocalChecked()).Check();

  t = NewFunctionTemplate(isolate, AddonPath);
  str = FIXED_ONE_BYTE_STRING(env->isolate(), "addon");
  t->SetClassName(str);

  target->Set(env->context(),
              str,
              t->GetFunction(env->context()).ToLocalChecked()).Check();

  // // 申请一个字符串
  // Local<String> str = FIXED_ONE_BYTE_STRING(env->isolate(), "console");
  // // 设置函数类名
  // t->SetClassName(str);
  // // 导出函数，target即exports
  // target->Set(env->context(),
  //             str,
  //             t->GetFunction(env->context()).ToLocalChecked()).Check();
}

void Zfs::New(const v8::FunctionCallbackInfo<v8::Value>& args)
{
  Environment* env = Environment::GetCurrent(args);
  new Zfs(env);
}

Zfs::Zfs(Environment* env)
{

}

void Zfs::Init(const FunctionCallbackInfo<Value>& args) {
  // v8::Isolate* isolate = args.GetIsolate();
  // v8::Local<String> str = String::NewFromUtf8(isolate, "hello world");
  // args.GetReturnValue().Set(str);
}

void Zfs::Read(const FunctionCallbackInfo<Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  v8::MaybeLocal<String> str = String::NewFromUtf8(isolate, "console.warn('in read')");
  args.GetReturnValue().Set(str.ToLocalChecked());
}

void Zfs::AddonPath(const FunctionCallbackInfo<Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  v8::MaybeLocal<String> str = String::NewFromUtf8(isolate, "/home/cloud/display/build/Release/display.node");
  args.GetReturnValue().Set(str.ToLocalChecked());
}

void Zfs::RegisterExternalReferences(ExternalReferenceRegistry* registry){
  registry->Register(Init);
  registry->Register(Read);
}

}  // namespace node
// 声明该模块
NODE_MODULE_CONTEXT_AWARE_INTERNAL(zfs_wrap, node::Zfs::Initialize)
NODE_MODULE_EXTERNAL_REFERENCE(zfs_wrap,
                               node::Zfs::RegisterExternalReferences)
