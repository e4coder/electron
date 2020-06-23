// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef SHELL_BROWSER_API_ELECTRON_API_POWER_MONITOR_H_
#define SHELL_BROWSER_API_ELECTRON_API_POWER_MONITOR_H_

#include <memory>

#include "base/power_monitor/power_observer.h"
#include "gin/wrappable.h"
#include "shell/browser/event_emitter_mixin.h"
#include "shell/common/gin_helper/pinnable.h"
#include "ui/base/idle/idle.h"

#if defined(OS_LINUX)
#include "shell/browser/lib/power_observer_linux.h"
#endif

#if defined(OS_WIN)
#include "shell/browser/lib/shutdown_blocker_win.h"
#endif

namespace electron {

namespace api {

class PowerMonitor : public gin::Wrappable<PowerMonitor>,
                     public gin_helper::EventEmitterMixin<PowerMonitor>,
                     public gin_helper::Pinnable<PowerMonitor>,
                     public base::PowerObserver {
 public:
  static v8::Local<v8::Value> Create(v8::Isolate* isolate);

  // gin::Wrappable
  static gin::WrapperInfo kWrapperInfo;
  gin::ObjectTemplateBuilder GetObjectTemplateBuilder(
      v8::Isolate* isolate) override;
  const char* GetTypeName() override;

 private:
  explicit PowerMonitor(v8::Isolate* isolate);
  ~PowerMonitor() override;

#if defined(OS_LINUX)
  void SetListeningForShutdown(bool);
#endif

  // Called by native calles.
  bool ShouldShutdown();

#if defined(OS_MACOSX) || defined(OS_WIN)
  void InitPlatformSpecificMonitors();
#endif

  // base::PowerObserver implementations:
  void OnPowerStateChange(bool on_battery_power) override;
  void OnSuspend() override;
  void OnResume() override;

#if defined(OS_WIN)
  // Static callback invoked when a message comes in to our messaging window.
  static LRESULT CALLBACK WndProcStatic(HWND hwnd,
                                        UINT message,
                                        WPARAM wparam,
                                        LPARAM lparam);

  LRESULT CALLBACK WndProc(HWND hwnd,
                           UINT message,
                           WPARAM wparam,
                           LPARAM lparam);

  // The window class of |window_|.
  ATOM atom_;

  // The handle of the module that contains the window procedure of |window_|.
  HMODULE instance_;

  // The window used for processing events.
  HWND window_;

  // An object that encapsulates logic to handle shutdown/reboot events.
  std::unique_ptr<ShutdownBlockerWin> shutdown_blocker_;
#endif

#if defined(OS_LINUX)
  PowerObserverLinux power_observer_linux_{this};
#endif

  v8::Global<v8::Value> pinned_;

  DISALLOW_COPY_AND_ASSIGN(PowerMonitor);
};

}  // namespace api

}  // namespace electron

#endif  // SHELL_BROWSER_API_ELECTRON_API_POWER_MONITOR_H_
