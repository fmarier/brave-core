/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/safe_browsing/download_protection/check_client_download_request.h"
#include "components/safe_browsing/core/proto/csd.pb.h"

namespace safe_browsing {

class BraveCheckClientDownloadRequest : public CheckClientDownloadRequest {
 public:
  BraveCheckClientDownloadRequest(
      download::DownloadItem* item,
      CheckDownloadRepeatingCallback callback,
      DownloadProtectionService* service,
      scoped_refptr<SafeBrowsingDatabaseManager> database_manager,
      scoped_refptr<BinaryFeatureExtractor> binary_feature_extractor);
  ~BraveCheckClientDownloadRequest() override;

 private:
  void NotifySendRequest(const ClientDownloadRequest* request) override;

  DISALLOW_COPY_AND_ASSIGN(BraveCheckClientDownloadRequest);
};

BraveCheckClientDownloadRequest::BraveCheckClientDownloadRequest(
    download::DownloadItem* item,
    CheckDownloadRepeatingCallback callback,
    DownloadProtectionService* service,
    scoped_refptr<SafeBrowsingDatabaseManager> database_manager,
    scoped_refptr<BinaryFeatureExtractor> binary_feature_extractor)
    : CheckClientDownloadRequest(item,
                                 callback,
                                 service,
                                 database_manager,
                                 binary_feature_extractor) {}

BraveCheckClientDownloadRequest::~BraveCheckClientDownloadRequest() = default;

void BraveCheckClientDownloadRequest::NotifySendRequest(
    const ClientDownloadRequest* request) {
  ClientDownloadRequest* mutable_request =
      const_cast<ClientDownloadRequest*>(request);
  mutable_request->set_url("");  // URL must be present or we get a 400.
  mutable_request->clear_file_basename();
  mutable_request->clear_locale();
  mutable_request->clear_resources();  // Contains URLs and referrers
  mutable_request->clear_referrer_chain();

  // Filter binaries within archives.
  for (int i = 0; i < mutable_request->archived_binary_size(); i++) {
    ClientDownloadRequest_ArchivedBinary* archived_binary =
        mutable_request->mutable_archived_binary(i);
    archived_binary->clear_file_basename();
  }

  CheckClientDownloadRequest::NotifySendRequest(request);
}

}  // namespace safe_browsing

#define CheckClientDownloadRequestBase CheckClientDownloadDONTTOUCHRequestBase
#define CheckClientDownloadRequest BraveCheckClientDownloadRequest
#define CheckClientDownloadDONTTOUCHRequestBase CheckClientDownloadRequestBase

#include "../../../../../../chrome/browser/safe_browsing/download_protection/download_protection_service.cc"  // NOLINT

#undef CheckClientDownloadRequestBase
#undef CheckClientDownloadRequest
#undef CheckClientDownloadDONTTOUCHRequestBase
