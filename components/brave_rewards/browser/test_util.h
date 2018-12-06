/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_REWARDS_BROWSER_TEST_UTIL_H_
#define BRAVE_COMPONENTS_BRAVE_REWARDS_BROWSER_TEST_UTIL_H_

#include <memory>

#include "bat/ledger/ledger.h"
#include "base/files/file_path.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

class KeyedService;
class Profile;

namespace brave_rewards {

class MockLedgerClient : public ledger::Ledger {
 public:
  MockLedgerClient();
  ~MockLedgerClient() override;

  MOCK_METHOD2(OnLoad, void(const ledger::VisitData& visit_data, const uint64_t& current_time));
};

std::unique_ptr<Profile> CreateBraveRewardsProfile(const base::FilePath& path);

}  // namespace brave_rewards

#endif  // BRAVE_COMPONENTS_BRAVE_REWARDS_BROWSER_TEST_UTIL_H_
