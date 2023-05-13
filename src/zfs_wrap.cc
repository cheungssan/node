#include "zfs_wrap.h"
#include "env-inl.h"
#include "minizip/unzip.h"
#include "node_external_reference.h"
#include "node_internals.h"
#include "util-inl.h"

namespace node {

using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::MaybeLocal;
using v8::Object;
using v8::String;
using v8::Value;

static std::map<std::string, char*> fs_;

void Zfs::Initialize(Local<Object> target,
                     Local<Value> unused,
                     Local<Context> context,
                     void* priv) {
  Environment* env = Environment::GetCurrent(context);
  Isolate* isolate = env->isolate();

  // Local<FunctionTemplate> constructor = NewFunctionTemplate(isolate, New);

  // SetProtoMethod(isolate, constructor, "init", Init);

  // SetConstructorFunction(context, target, "Zfs", constructor);

  Local<FunctionTemplate> t = NewFunctionTemplate(isolate, Init);
  Local<String> str = FIXED_ONE_BYTE_STRING(env->isolate(), "init");
  t->SetClassName(str);

  target
      ->Set(
          env->context(), str, t->GetFunction(env->context()).ToLocalChecked())
      .Check();

  t = NewFunctionTemplate(isolate, Read);
  str = FIXED_ONE_BYTE_STRING(env->isolate(), "read");
  t->SetClassName(str);

  target
      ->Set(
          env->context(), str, t->GetFunction(env->context()).ToLocalChecked())
      .Check();

  t = NewFunctionTemplate(isolate, AddonPath);
  str = FIXED_ONE_BYTE_STRING(env->isolate(), "addon");
  t->SetClassName(str);

  target
      ->Set(
          env->context(), str, t->GetFunction(env->context()).ToLocalChecked())
      .Check();

  // // 申请一个字符串
  // Local<String> str = FIXED_ONE_BYTE_STRING(env->isolate(), "console");
  // // 设置函数类名
  // t->SetClassName(str);
  // // 导出函数，target即exports
  // target->Set(env->context(),
  //             str,
  //             t->GetFunction(env->context()).ToLocalChecked()).Check();
}

void Zfs::New(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  new Zfs(env);
}

Zfs::Zfs(Environment* env) {}

bool init = false;

void Zfs::Init(const FunctionCallbackInfo<Value>& args) {
  const char* zipfile = "/home/cloud/lib.zip";
  unzFile uf = unzOpen64(zipfile);
  if (uf == NULL) {
    printf("unzip failed\n");
    return;
  }
  unz_global_info64 info;
  int err = unzGetGlobalInfo64(uf, &info);
  if (err != UNZ_OK) {
    printf("unzGetGlobalInfo64 failed\n");
    return;
  }

  v8::Isolate* isolate = args.GetIsolate();
  for (int i = 0; i < info.number_entry; i++) {
    char filename_inzip[256];
    unz_file_info64 file_info;
    err = unzGetCurrentFileInfo64(uf,
                                  &file_info,
                                  filename_inzip,
                                  sizeof(filename_inzip),
                                  NULL,
                                  0,
                                  NULL,
                                  0);
    if (err != UNZ_OK) {
      printf("unzGetCurrentFileInfo64 failed\n");
      return;
    }

    printf(" file zip : %s\n", filename_inzip);
    unzFile file;
    err = unzOpenCurrentFile(&file);
    char* content = new char[file_info.size_file_extra + 1];
    content[file_info.size_file_extra] = 0x0;
    unzReadCurrentFile(file, content, file_info.size_file_extra);
    std::string path(":/");
    path += filename_inzip;

    fs_[path] = content;

    if ((i + 1) < info.number_entry) {
      err = unzGoToNextFile(uf);
      if (err != UNZ_OK) {
        printf("error %d with zipfile in unzGoToNextFile\n", err);
        break;
      }
    }
  }

  printf("hello\n");
}

void Zfs::Read(const FunctionCallbackInfo<Value>& args) {
  if (!init) {
    init = true;

    const char* zipfile = "/home/cloud/lib.zip";
    unzFile uf = unzOpen64(zipfile);
    if (uf == NULL) {
      printf("unzip failed\n");
      return;
    }
    unz_global_info64 info;
    int err = unzGetGlobalInfo64(uf, &info);
    if (err != UNZ_OK) {
      printf("unzGetGlobalInfo64 failed\n");
      return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    for (int i = 0; i < info.number_entry; i++) {
      char filename_inzip[256];
      unz_file_info64 file_info;
      err = unzGetCurrentFileInfo64(uf,
                                    &file_info,
                                    filename_inzip,
                                    sizeof(filename_inzip),
                                    NULL,
                                    0,
                                    NULL,
                                    0);
      if (err != UNZ_OK) {
        printf("unzGetCurrentFileInfo64 failed\n");
        return;
      }

      printf(" file zip : %s\n", filename_inzip);
      int len = strlen(filename_inzip);
      if (filename_inzip[len - 1] == '/') {
        err = unzGoToNextFile(uf);
        if (err != UNZ_OK) {
          printf("error %d with zipfile in unzGoToNextFile\n", err);
          break;
        }
        continue;
      }

      unzFile file;
      err = unzOpenCurrentFile(uf);
      char* content = new char[file_info.uncompressed_size + 1];
      content[file_info.uncompressed_size] = 0x0;
      unzReadCurrentFile(uf, content, file_info.uncompressed_size);
      printf("unzReadCurrentFile 2\n");
      std::string path(":/");
      path += filename_inzip;

      printf("path %s\n", path.c_str());
      content[file_info.uncompressed_size] = 0x0;
      fs_[path] = content;
      printf("path %s, content %s$\n", path.c_str(), content);
      if ((i + 1) < info.number_entry) {
        err = unzGoToNextFile(uf);
        if (err != UNZ_OK) {
          printf("error %d with zipfile in unzGoToNextFile\n", err);
          break;
        }
      }
    }
  }

  Environment* env = Environment::GetCurrent(args);
  node::Utf8Value filename(env->isolate(), args[0]);
  std::string key(*filename);
  auto iter = fs_.find(key);
  if (iter == fs_.end()) {
    //
    return;
  }
  v8::Isolate* isolate = args.GetIsolate();
  v8::MaybeLocal<String> str = String::NewFromUtf8(isolate, iter->second);
  args.GetReturnValue().Set(str.ToLocalChecked());
}

void Zfs::AddonPath(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  node::Utf8Value filename(env->isolate(), args[0]);
  std::string key(*filename);
  auto iter = fs_.find(key);
  if (iter == fs_.end()) {
    //
    return;
  }

  v8::Isolate* isolate = args.GetIsolate();
  // fixme
  v8::MaybeLocal<String> str = String::NewFromUtf8(isolate, iter->second);
  args.GetReturnValue().Set(str.ToLocalChecked());
}

void Zfs::RegisterExternalReferences(ExternalReferenceRegistry* registry) {
  registry->Register(Init);
  registry->Register(Read);
}

}  // namespace node
// 声明该模块
NODE_MODULE_CONTEXT_AWARE_INTERNAL(zfs_wrap, node::Zfs::Initialize)
NODE_MODULE_EXTERNAL_REFERENCE(zfs_wrap, node::Zfs::RegisterExternalReferences)
