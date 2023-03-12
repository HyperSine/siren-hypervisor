#pragma once
#include <specstrings.h>
#include <kernelspecs.h>

#if defined(_X86_)
//
// Interrupt Request Level definitions
//
#ifndef CMCI_LEVEL
#define CMCI_LEVEL 5                // CMCI handler level
#endif

#ifndef PROFILE_LEVEL
#define PROFILE_LEVEL 27            // timer used for profiling.
#endif

#ifndef CLOCK1_LEVEL
#define CLOCK1_LEVEL 28             // Interval clock 1 level - Not used on x86
#endif

#ifndef CLOCK2_LEVEL
#define CLOCK2_LEVEL 28             // Interval clock 2 level
#endif

#ifndef IPI_LEVEL
#define IPI_LEVEL 29                // Interprocessor interrupt level
#endif

#ifndef POWER_LEVEL
#define POWER_LEVEL 30              // Power failure level
#endif

#ifndef CLOCK_LEVEL
#define CLOCK_LEVEL                 (CLOCK2_LEVEL)
#endif
#endif

#if defined(_AMD64_) 
//
// Interrupt Request Level definitions
//
#ifndef CMCI_LEVEL
#define CMCI_LEVEL 5                    // CMCI handler level
#endif

#ifndef CLOCK_LEVEL
#define CLOCK_LEVEL 13                  // Interval clock level
#endif

#ifndef IPI_LEVEL
#define IPI_LEVEL 14                    // Interprocessor interrupt level
#endif

#ifndef DRS_LEVEL
#define DRS_LEVEL 14                    // Deferred Recovery Service level
#endif

#ifndef POWER_LEVEL
#define POWER_LEVEL 14                  // Power failure level
#endif

#ifndef PROFILE_LEVEL
#define PROFILE_LEVEL 15                // timer used for profiling.
#endif
#endif

#if defined(_ARM_) 
//
// Interrupt Request Level definitions
//
#ifndef CLOCK_LEVEL
#define CLOCK_LEVEL 13                  // Interval clock level
#endif

#ifndef IPI_LEVEL
#define IPI_LEVEL 14                    // Interprocessor interrupt level
#endif

#ifndef DRS_LEVEL
#define DRS_LEVEL 14                    // Deferred Recovery Service level
#endif

#ifndef POWER_LEVEL
#define POWER_LEVEL 14                  // Power failure level
#endif

#ifndef PROFILE_LEVEL
#define PROFILE_LEVEL 15                // timer used for profiling.
#endif
#endif

#if defined(_ARM64_) 
//
// Interrupt Request Level definitions
//
#ifndef CLOCK_LEVEL
#define CLOCK_LEVEL 13                  // Interval clock level
#endif

#ifndef IPI_LEVEL
#define IPI_LEVEL 14                    // Interprocessor interrupt level
#endif

#ifndef DRS_LEVEL
#define DRS_LEVEL 14                    // Deferred Recovery Service level
#endif

#ifndef POWER_LEVEL
#define POWER_LEVEL 14                  // Power failure level
#endif

#ifndef PROFILE_LEVEL
#define PROFILE_LEVEL 15                // timer used for profiling.
#endif
#endif
