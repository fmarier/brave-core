/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/modules/credentialmanager/credentials_container.h"

#include "gin/converter.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise_resolver.h"
#include "third_party/blink/renderer/platform/bindings/v8_per_isolate_data.h"
#include "v8/include/v8.h"

namespace blink {

  CredentialsContainer::CredentialsContainer() = default;

  ScriptPromise CredentialsContainer::get(
      ScriptState* script_state,
      const CredentialRequestOptions* options) {
    ScriptPromiseResolver* resolver =
        ScriptPromiseResolver::Create(script_state);
    ScriptPromise promise = resolver->Promise();
    v8::Isolate* isolate = script_state->GetIsolate();
    resolver->Resolve(v8::Null(isolate));
    return promise;
  }

  ScriptPromise CredentialsContainer::store(ScriptState* script_state,
      Credential* credential) {
    ScriptPromiseResolver* resolver =
        ScriptPromiseResolver::Create(script_state);
    ScriptPromise promise = resolver->Promise();
    v8::Isolate* isolate = script_state->GetIsolate();
    resolver->Resolve(v8::Null(isolate));
    return promise;
  }

  ScriptPromise CredentialsContainer::preventSilentAccess(
      ScriptState* script_state) {
    ScriptPromiseResolver* resolver =
        ScriptPromiseResolver::Create(script_state);
    ScriptPromise promise = resolver->Promise();
    resolver->Resolve();
    return promise;
  }

  CredentialsContainer* CredentialsContainer::Create() {
    return MakeGarbageCollected<CredentialsContainer>();
  }

  ScriptPromise CredentialsContainer::create(ScriptState* script_state,
      const CredentialCreationOptions* options,
      ExceptionState& exception_state) {
    ScriptPromiseResolver* resolver =
        ScriptPromiseResolver::Create(script_state);
    ScriptPromise promise = resolver->Promise();
    v8::Isolate* isolate = script_state->GetIsolate();
    resolver->Resolve(v8::Null(isolate));
    return promise;
  }
}  // namespace blink
