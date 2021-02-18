/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright 2020 TeMoto Telerobotics
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef TEMOTO_ACTION_ENGINE__THREADED_ACTION_INDEXER_H
#define TEMOTO_ACTION_ENGINE__THREADED_ACTION_INDEXER_H

#include "temoto_action_engine/action_indexer.h"
#include <thread>
#include <mutex>

namespace temoto_action_assistant
{
class ThreadedActionIndexer
{
public:
  ThreadedActionIndexer(const std::string& temoto_actions_path = "");

  unsigned int getActionCount() const;

  std::vector<std::string> getUmrfNames() const;

  bool hasUmrf(const std::string& umrf_name) const;

  UmrfNode getUmrf(const std::string& umrf_name) const;

  ~ThreadedActionIndexer();

private:
  ActionIndexer action_indexer_;
  std::thread indexing_thread_;
  bool stop_indexing_ = false;
  std::vector<UmrfNode> umrfs_;
  mutable std::mutex umrfs_mutex_;

  void startIndexing();

  void stopIndexing();
};
} // temoto_action_assistant namespace
#endif