#include "temoto_action_assistant/threaded_action_indexer.h"
#include <algorithm>

namespace temoto_action_assistant
{

ThreadedActionIndexer::ThreadedActionIndexer(const std::string& temoto_actions_path)
{
  if (!temoto_actions_path.empty())
  {
    action_indexer_.addActionPath(temoto_actions_path);
  }

  indexing_thread_ = std::thread([this]{startIndexing();});
}

void ThreadedActionIndexer::startIndexing()
{
  while (!stop_indexing_)
  {
    action_indexer_.indexActions();
    {
      std::lock_guard<std::mutex> umrfs_lock(umrfs_mutex_);
      umrfs_ = action_indexer_.getUmrfs();

    }
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));
  }
}

void ThreadedActionIndexer::stopIndexing()
{
  stop_indexing_ = true;
  while (!indexing_thread_.joinable())
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  indexing_thread_.join();
}

bool ThreadedActionIndexer::hasUmrf(const std::string& umrf_name) const
{
  std::lock_guard<std::mutex> umrfs_lock(umrfs_mutex_);
  for (const auto& umrf : umrfs_)
  {
    if (umrf.getName() == umrf_name)
    {
      return true;
    }
  }
  return false;
}

Umrf ThreadedActionIndexer::getUmrf(const std::string& umrf_name) const
{
  std::lock_guard<std::mutex> umrfs_lock(umrfs_mutex_);
  const auto& umrf_it = std::find_if(umrfs_.begin(), umrfs_.end(),
  [&](const Umrf& umrf)
  {
    if (umrf.getPackageName() == umrf_name)
      return true;
    else
      return false;
  });

  if (umrf_it == umrfs_.end())
  {
    return Umrf();
  }
  else
  {
    return *umrf_it;
  }
}

std::vector<std::string> ThreadedActionIndexer::getUmrfNames() const
{
  std::lock_guard<std::mutex> umrfs_lock(umrfs_mutex_);
  std::vector<std::string> umrf_names;

  for (const auto& umrf : umrfs_)
  {
    umrf_names.push_back(umrf.getPackageName());  
  }

  return umrf_names;
}

unsigned int ThreadedActionIndexer::getActionCount() const
{
  std::lock_guard<std::mutex> umrfs_lock(umrfs_mutex_);
  return umrfs_.size();
}

ThreadedActionIndexer::~ThreadedActionIndexer()
{
  stopIndexing();
}

} // temoto_action_assistant namespace