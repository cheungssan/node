
#ifndef __ZFS_H__
#define __ZFS_H__

#include "v8.h"

namespace node{
    class Environment;
    class ExternalReferenceRegistry;
    class Zfs{
        public:
        static void Initialize(v8::Local<v8::Object> targe,
                                v8::Local<v8::Value> unused,
                         v8::Local<v8::Context> context,
                         void* priv);
        static void RegisterExternalReferences(ExternalReferenceRegistry* registry);
        private:
        Zfs(Environment* env);
static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Init(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Read(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void AddonPath(const v8::FunctionCallbackInfo<v8::Value>& args);
    };
};

#endif
