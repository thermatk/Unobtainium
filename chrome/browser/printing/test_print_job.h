// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_PRINTING_TEST_PRINT_JOB_H_
#define CHROME_BROWSER_PRINTING_TEST_PRINT_JOB_H_

#include <memory>

#include "base/callback.h"
#include "base/macros.h"
#include "build/build_config.h"
#include "chrome/browser/printing/print_job.h"
#include "chrome/browser/printing/print_job_worker_owner.h"
#include "printing/print_settings.h"

namespace printing {

class TestPrintJob : public PrintJob {
 public:
  // Create a empty PrintJob. When initializing with this constructor,
  // post-constructor initialization must be done with Initialize().
  TestPrintJob() = default;

  // Getters for values stored by TestPrintJob in Start...Converter functions.
  const gfx::Size& page_size() const { return page_size_; }
  const gfx::Rect& content_area() const { return content_area_; }
  const gfx::Point& offsets() const { return offsets_; }
#if defined(OS_WIN)
  PrintSettings::PrinterType type() const { return type_; }
#endif

  // content::NotificationObserver implementation. Deliberately empty.
  void Observe(int type,
               const content::NotificationSource& source,
               const content::NotificationDetails& details) override {}

  // All remaining functions are PrintJob implementation.
  void Initialize(PrintJobWorkerOwner* job,
                  const base::string16& name,
                  int page_count) override;

  // Sets |job_pending_| to true.
  void StartPrinting() override;

  // Sets |job_pending_| to false and deletes the worker.
  void Stop() override;

  // Sets |job_pending_| to false and deletes the worker.
  void Cancel() override;

  // Intentional no-op, returns true.
  bool FlushJob(base::TimeDelta timeout) override;

#if defined(OS_WIN)
  // These functions fill in the values pointed to by |page_size_|,
  // |content_area_|, |offsets_| and |type_| based on the arguments passed
  // to them.
  void StartPdfToEmfConversion(
      const scoped_refptr<base::RefCountedMemory>& bytes,
      const gfx::Size& page_size,
      const gfx::Rect& content_area,
      bool print_text_with_gdi) override;

  void StartPdfToPostScriptConversion(
      const scoped_refptr<base::RefCountedMemory>& bytes,
      const gfx::Rect& content_area,
      const gfx::Point& physical_offset,
      bool ps_level2) override;

  void StartPdfToTextConversion(
      const scoped_refptr<base::RefCountedMemory>& bytes,
      const gfx::Size& page_size) override;
#endif  // defined(OS_WIN)

 protected:
  ~TestPrintJob() override;

 private:
  gfx::Size page_size_;
  gfx::Rect content_area_;
  gfx::Point offsets_;
#if defined(OS_WIN)
  PrintSettings::PrinterType type_;
#endif
};

}  // namespace printing

#endif  // CHROME_BROWSER_PRINTING_TEST_PRINT_JOB_H_