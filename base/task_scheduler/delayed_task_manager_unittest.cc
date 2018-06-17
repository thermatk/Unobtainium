// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/task_scheduler/delayed_task_manager.h"

#include <memory>
#include <utility>

#include "base/bind.h"
#include "base/memory/ptr_util.h"
#include "base/memory/ref_counted.h"
#include "base/task_scheduler/task.h"
#include "base/test/test_mock_time_task_runner.h"
#include "base/time/time.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace base {
namespace internal {
namespace {

constexpr TimeDelta kLongDelay = TimeDelta::FromHours(1);

class MockTask {
 public:
  MOCK_METHOD0(Run, void());
};

void RunTask(Task task) {
  std::move(task.task).Run();
}

class TaskSchedulerDelayedTaskManagerTest : public testing::Test {
 protected:
  TaskSchedulerDelayedTaskManagerTest()
      : delayed_task_manager_(
            service_thread_task_runner_->DeprecatedGetMockTickClock()),
        task_(FROM_HERE,
              BindOnce(&MockTask::Run, Unretained(&mock_task_)),
              TaskTraits(),
              kLongDelay) {
    // The constructor of Task computes |delayed_run_time| by adding |delay| to
    // the real time. Recompute it by adding |delay| to the mock time.
    task_.delayed_run_time =
        service_thread_task_runner_->GetMockTickClock()->NowTicks() +
        kLongDelay;
  }
  ~TaskSchedulerDelayedTaskManagerTest() override = default;

  const scoped_refptr<TestMockTimeTaskRunner> service_thread_task_runner_ =
      MakeRefCounted<TestMockTimeTaskRunner>();
  DelayedTaskManager delayed_task_manager_;
  testing::StrictMock<MockTask> mock_task_;
  Task task_;

 private:
  DISALLOW_COPY_AND_ASSIGN(TaskSchedulerDelayedTaskManagerTest);
};

}  // namespace

// Verify that a delayed task isn't forwarded before Start().
TEST_F(TaskSchedulerDelayedTaskManagerTest, DelayedTaskDoesNotRunBeforeStart) {
  // Send |task| to the DelayedTaskManager.
  delayed_task_manager_.AddDelayedTask(std::move(task_), BindOnce(&RunTask));

  // Fast-forward time until the task is ripe for execution. Since Start() has
  // not been called, the task should not be forwarded to RunTask() (MockTask is
  // a StrictMock without expectations so test will fail if RunTask() runs it).
  service_thread_task_runner_->FastForwardBy(kLongDelay);
}

// Verify that a delayed task added before Start() and whose delay expires after
// Start() is forwarded when its delay expires.
TEST_F(TaskSchedulerDelayedTaskManagerTest,
       DelayedTaskPostedBeforeStartExpiresAfterStartRunsOnExpire) {
  // Send |task| to the DelayedTaskManager.
  delayed_task_manager_.AddDelayedTask(std::move(task_), BindOnce(&RunTask));

  delayed_task_manager_.Start(service_thread_task_runner_);

  // Run tasks on the service thread. Don't expect any forwarding to
  // |task_target_| since the task isn't ripe for execution.
  service_thread_task_runner_->RunUntilIdle();

  // Fast-forward time until the task is ripe for execution. Expect the task to
  // be forwarded to RunTask().
  EXPECT_CALL(mock_task_, Run());
  service_thread_task_runner_->FastForwardBy(kLongDelay);
}

// Verify that a delayed task added before Start() and whose delay expires
// before Start() is forwarded when Start() is called.
TEST_F(TaskSchedulerDelayedTaskManagerTest,
       DelayedTaskPostedBeforeStartExpiresBeforeStartRunsOnStart) {
  // Send |task| to the DelayedTaskManager.
  delayed_task_manager_.AddDelayedTask(std::move(task_), BindOnce(&RunTask));

  // Run tasks on the service thread. Don't expect any forwarding to
  // |task_target_| since the task isn't ripe for execution.
  service_thread_task_runner_->RunUntilIdle();

  // Fast-forward time until the task is ripe for execution. Don't expect the
  // task to be forwarded since Start() hasn't been called yet.
  service_thread_task_runner_->FastForwardBy(kLongDelay);

  // Start the DelayedTaskManager. Expect the task to be forwarded to RunTask().
  EXPECT_CALL(mock_task_, Run());
  delayed_task_manager_.Start(service_thread_task_runner_);
  service_thread_task_runner_->RunUntilIdle();
}

// Verify that a delayed task added after Start() isn't forwarded before it is
// ripe for execution.
TEST_F(TaskSchedulerDelayedTaskManagerTest, DelayedTaskDoesNotRunTooEarly) {
  delayed_task_manager_.Start(service_thread_task_runner_);

  // Send |task| to the DelayedTaskManager.
  delayed_task_manager_.AddDelayedTask(std::move(task_), BindOnce(&RunTask));

  // Run tasks that are ripe for execution. Don't expect any forwarding to
  // RunTask().
  service_thread_task_runner_->RunUntilIdle();
}

// Verify that a delayed task added after Start() is forwarded when it is ripe
// for execution.
TEST_F(TaskSchedulerDelayedTaskManagerTest, DelayedTaskRunsAfterDelay) {
  delayed_task_manager_.Start(service_thread_task_runner_);

  // Send |task| to the DelayedTaskManager.
  delayed_task_manager_.AddDelayedTask(std::move(task_), BindOnce(&RunTask));

  // Fast-forward time. Expect the task to be forwarded to RunTask().
  EXPECT_CALL(mock_task_, Run());
  service_thread_task_runner_->FastForwardBy(kLongDelay);
}

// Verify that multiple delayed tasks added after Start() are forwarded when
// they are ripe for execution.
TEST_F(TaskSchedulerDelayedTaskManagerTest, DelayedTasksRunAfterDelay) {
  delayed_task_manager_.Start(service_thread_task_runner_);

  testing::StrictMock<MockTask> mock_task_a;
  Task task_a(FROM_HERE, BindOnce(&MockTask::Run, Unretained(&mock_task_a)),
              TaskTraits(), TimeDelta::FromHours(1));

  testing::StrictMock<MockTask> mock_task_b;
  Task task_b(FROM_HERE, BindOnce(&MockTask::Run, Unretained(&mock_task_b)),
              TaskTraits(), TimeDelta::FromHours(2));

  testing::StrictMock<MockTask> mock_task_c;
  Task task_c(FROM_HERE, BindOnce(&MockTask::Run, Unretained(&mock_task_c)),
              TaskTraits(), TimeDelta::FromHours(1));

  // Send tasks to the DelayedTaskManager.
  delayed_task_manager_.AddDelayedTask(std::move(task_a), BindOnce(&RunTask));
  delayed_task_manager_.AddDelayedTask(std::move(task_b), BindOnce(&RunTask));
  delayed_task_manager_.AddDelayedTask(std::move(task_c), BindOnce(&RunTask));

  // Run tasks that are ripe for execution on the service thread. Don't expect
  // any call to RunTask().
  service_thread_task_runner_->RunUntilIdle();

  // Fast-forward time. Expect |task_a| and |task_c| to be forwarded to
  // |task_target_|.
  EXPECT_CALL(mock_task_a, Run());
  EXPECT_CALL(mock_task_c, Run());
  service_thread_task_runner_->FastForwardBy(TimeDelta::FromHours(1));
  testing::Mock::VerifyAndClear(&mock_task_a);
  testing::Mock::VerifyAndClear(&mock_task_c);

  // Fast-forward time. Expect |task_b| to be forwarded to RunTask().
  EXPECT_CALL(mock_task_b, Run());
  service_thread_task_runner_->FastForwardBy(TimeDelta::FromHours(1));
  testing::Mock::VerifyAndClear(&mock_task_b);
}

}  // namespace internal
}  // namespace base