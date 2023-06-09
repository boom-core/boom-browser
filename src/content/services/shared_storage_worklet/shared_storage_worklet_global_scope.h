// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SERVICES_SHARED_STORAGE_WORKLET_SHARED_STORAGE_WORKLET_GLOBAL_SCOPE_H_
#define CONTENT_SERVICES_SHARED_STORAGE_WORKLET_SHARED_STORAGE_WORKLET_GLOBAL_SCOPE_H_

#include "content/common/content_export.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "services/network/public/mojom/url_loader_factory.mojom.h"
#include "third_party/blink/public/mojom/private_aggregation/private_aggregation_host.mojom-forward.h"
#include "third_party/blink/public/mojom/shared_storage/shared_storage_worklet_service.mojom.h"
#include "v8/include/v8-forward.h"
#include "v8/include/v8-persistent-handle.h"

namespace gin {
class IsolateHolder;
class Arguments;
}  // namespace gin

namespace blink {
class ModuleScriptDownloader;
}  // namespace blink

namespace shared_storage_worklet {

class UrlSelectionOperationHandler;
class UnnamedOperationHandler;
class Console;
class PrivateAggregation;
class SharedStorage;

// The global JS execution context for shared storage worklet. It holds a
// v8::Isolate and a v8::Context to execute all worklet operations. Members are
// initialized only after AddModule() succeeds.
// https://github.com/pythagoraskitty/shared-storage/blob/main/README.md
class CONTENT_EXPORT SharedStorageWorkletGlobalScope {
 public:
  SharedStorageWorkletGlobalScope(
      bool private_aggregation_permissions_policy_allowed,
      const absl::optional<std::u16string>& embedder_context);
  ~SharedStorageWorkletGlobalScope();

  void AddModule(
      mojo::PendingRemote<network::mojom::URLLoaderFactory>
          pending_url_loader_factory,
      blink::mojom::SharedStorageWorkletServiceClient* client,
      blink::mojom::PrivateAggregationHost* private_aggregation_host,
      const GURL& script_source_url,
      blink::mojom::SharedStorageWorkletService::AddModuleCallback callback);

  void OnModuleScriptDownloaded(
      blink::mojom::SharedStorageWorkletServiceClient* client,
      blink::mojom::PrivateAggregationHost* private_aggregation_host,
      const GURL& script_source_url,
      blink::mojom::SharedStorageWorkletService::AddModuleCallback callback,
      std::unique_ptr<std::string> response_body,
      std::string error_message);

  void RunURLSelectionOperation(const std::string& name,
                                const std::vector<GURL>& urls,
                                const std::vector<uint8_t>& serialized_data,
                                blink::mojom::SharedStorageWorkletService::
                                    RunURLSelectionOperationCallback callback);

  void RunOperation(
      const std::string& name,
      const std::vector<uint8_t>& serialized_data,
      blink::mojom::SharedStorageWorkletService::RunOperationCallback callback);

 private:
  void Register(gin::Arguments* args);

  friend class SharedStorageWorkletGlobalScopeTest;

  void FlushAndResetPrivateAggregation();

  v8::Isolate* Isolate();

  v8::Local<v8::Context> LocalContext();

  bool private_aggregation_permissions_policy_allowed_;

  std::unique_ptr<blink::ModuleScriptDownloader> module_script_downloader_;

  std::unique_ptr<gin::IsolateHolder> isolate_holder_;
  v8::Global<v8::Context> global_context_;

  std::unique_ptr<Console> console_;
  std::unique_ptr<PrivateAggregation> private_aggregation_;
  std::unique_ptr<SharedStorage> shared_storage_;

  std::unique_ptr<UrlSelectionOperationHandler>
      url_selection_operation_handler_;
  std::unique_ptr<UnnamedOperationHandler> unnamed_operation_handler_;

  std::map<std::string, v8::Global<v8::Function>> operation_definition_map_;

  // If this worklet is inside a fenced frame or a URN iframe,
  // `embedder_context_` represents any contextual information written to the
  // frame's `blink::FencedFrameConfig` by the embedder before navigation to the
  // config. `embedder_context_` is passed to the worklet upon initialization.
  absl::optional<std::u16string> embedder_context_;

  base::WeakPtrFactory<SharedStorageWorkletGlobalScope> weak_ptr_factory_{this};
};

}  // namespace shared_storage_worklet

#endif  // CONTENT_SERVICES_SHARED_STORAGE_WORKLET_SHARED_STORAGE_WORKLET_GLOBAL_SCOPE_H_
