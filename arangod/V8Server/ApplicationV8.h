////////////////////////////////////////////////////////////////////////////////
/// @brief V8 engine configuration
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2004-2013 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Dr. Frank Celler
/// @author Copyright 2011-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIAGENS_V8SERVER_APPLICATION_V8_H
#define TRIAGENS_V8SERVER_APPLICATION_V8_H 1

#include "ApplicationServer/ApplicationFeature.h"

#include <v8.h>

#include "Basics/ConditionVariable.h"
#include "V8/JSLoader.h"

// -----------------------------------------------------------------------------
// --SECTION--                                              forward declarations
// -----------------------------------------------------------------------------

extern "C" {
  struct TRI_server_s;
  struct TRI_vocbase_s;
}

namespace triagens {
  namespace basics {
    class Thread;
  }

// -----------------------------------------------------------------------------
// --SECTION--                                               class ApplicationV8
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

  namespace arango {

////////////////////////////////////////////////////////////////////////////////
/// @brief application simple user and session management feature
////////////////////////////////////////////////////////////////////////////////

    class ApplicationV8 : public rest::ApplicationFeature {
      private:
        ApplicationV8 (ApplicationV8 const&);
        ApplicationV8& operator= (ApplicationV8 const&);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                      public types
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 isolate and context
////////////////////////////////////////////////////////////////////////////////

        struct V8Context {

////////////////////////////////////////////////////////////////////////////////
/// @brief identifier
////////////////////////////////////////////////////////////////////////////////

          size_t _id;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 context
////////////////////////////////////////////////////////////////////////////////

          v8::Persistent<v8::Context> _context;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 isolate
////////////////////////////////////////////////////////////////////////////////

          v8::Isolate* _isolate;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 locker
////////////////////////////////////////////////////////////////////////////////

          v8::Locker* _locker;

////////////////////////////////////////////////////////////////////////////////
/// @brief adds a global method
///
/// Caller must hold the _contextCondition.
////////////////////////////////////////////////////////////////////////////////

          void addGlobalContextMethod (string const&);

////////////////////////////////////////////////////////////////////////////////
/// @brief executes all global methods
///
/// Caller must hold the _contextCondition.
////////////////////////////////////////////////////////////////////////////////

          void handleGlobalContextMethods ();

////////////////////////////////////////////////////////////////////////////////
/// @brief mutex to protect _globalMethods
////////////////////////////////////////////////////////////////////////////////

          basics::Mutex _globalMethodsLock;

////////////////////////////////////////////////////////////////////////////////
/// @brief open global methods
////////////////////////////////////////////////////////////////////////////////

          std::vector<std::string> _globalMethods;

////////////////////////////////////////////////////////////////////////////////
/// @brief number of requests since last GC of the context
////////////////////////////////////////////////////////////////////////////////

          size_t _dirt;

////////////////////////////////////////////////////////////////////////////////
/// @brief timestamp of last GC for the context
////////////////////////////////////////////////////////////////////////////////

          double _lastGcStamp;
        };

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief constructor
////////////////////////////////////////////////////////////////////////////////

        ApplicationV8 (struct TRI_server_s*);

////////////////////////////////////////////////////////////////////////////////
/// @brief destructor
////////////////////////////////////////////////////////////////////////////////

        ~ApplicationV8 ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief return the app-path
////////////////////////////////////////////////////////////////////////////////

        const std::string& appPath () const {
          return _appPath;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief return the dev-app-path
////////////////////////////////////////////////////////////////////////////////
        
        const std::string& devAppPath () const {
          return _devAppPath;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the concurrency
////////////////////////////////////////////////////////////////////////////////

        void setConcurrency (size_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the database
////////////////////////////////////////////////////////////////////////////////

        void setVocbase (struct TRI_vocbase_s*);

////////////////////////////////////////////////////////////////////////////////
/// @brief enable the database version check
////////////////////////////////////////////////////////////////////////////////

        void performUpgrade ();

////////////////////////////////////////////////////////////////////////////////
/// @brief disable the database version check
////////////////////////////////////////////////////////////////////////////////

        void skipUpgrade ();

////////////////////////////////////////////////////////////////////////////////
/// @brief enters an context
////////////////////////////////////////////////////////////////////////////////

        V8Context* enterContext (TRI_vocbase_s*, 
                                 bool,
                                 bool);

////////////////////////////////////////////////////////////////////////////////
/// @brief exists an context
////////////////////////////////////////////////////////////////////////////////

        void exitContext (V8Context*);
        
////////////////////////////////////////////////////////////////////////////////
/// @brief adds a global context functions to be executed asap
////////////////////////////////////////////////////////////////////////////////

        void addGlobalContextMethod (string const&);

////////////////////////////////////////////////////////////////////////////////
/// @brief runs the garbage collection
////////////////////////////////////////////////////////////////////////////////

        void collectGarbage ();

////////////////////////////////////////////////////////////////////////////////
/// @brief disables actions
////////////////////////////////////////////////////////////////////////////////

        void disableActions ();

////////////////////////////////////////////////////////////////////////////////
/// @brief enables development mode
////////////////////////////////////////////////////////////////////////////////

        void enableDevelopmentMode ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                        ApplicationFeature methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ApplicationServer
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        void setupOptions (map<string, basics::ProgramOptionsDescription>&);

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        bool prepare ();

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        bool start ();

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        void close ();

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

        void stop ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                   private methods
// -----------------------------------------------------------------------------

      private:

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief determine which of the free contexts should be picked for the GC
////////////////////////////////////////////////////////////////////////////////

        V8Context* pickContextForGc ();

////////////////////////////////////////////////////////////////////////////////
/// @brief prepares a V8 instance
////////////////////////////////////////////////////////////////////////////////

        bool prepareV8Instance (size_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief shut downs a V8 instances
////////////////////////////////////////////////////////////////////////////////

        void shutdownV8Instance (size_t);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief server object
////////////////////////////////////////////////////////////////////////////////

        struct TRI_server_s* _server;

////////////////////////////////////////////////////////////////////////////////
/// @brief path to the directory containing the startup scripts
///
/// @CMDOPT{\--javascript.startup-directory @CA{directory}}
///
/// Specifies the @CA{directory} path to the JavaScript files used for
/// bootstraping.
////////////////////////////////////////////////////////////////////////////////

        string _startupPath;

////////////////////////////////////////////////////////////////////////////////
/// @brief semicolon separated list of module directories
///
/// This variable is automatically set based on the value of 
/// `--javascript.startup-directory`.
////////////////////////////////////////////////////////////////////////////////

        string _modulesPath;

////////////////////////////////////////////////////////////////////////////////
/// @brief path to the system action directory
///
/// This variable is automatically set based on the value of 
/// `--javascript.startup-directory`.
////////////////////////////////////////////////////////////////////////////////

        string _actionPath;

////////////////////////////////////////////////////////////////////////////////
/// @brief semicolon separated list of application directories
///
/// @CMDOPT{\--javascript.app-path @CA{directory}}
///
/// Specifies the @CA{directory} path where the applications are located.
/// Multiple paths can be specified separated with commas.
////////////////////////////////////////////////////////////////////////////////

        string _appPath;

////////////////////////////////////////////////////////////////////////////////
/// @brief semicolon separated list of application directories
///
/// @CMDOPT{\--javascript.dev-app-path @CA{directory}}
///
/// Specifies the @CA{directory} path where the development applications are
/// located.  Multiple paths can be specified separated with commas. Never use
/// this option for production.
////////////////////////////////////////////////////////////////////////////////

        string _devAppPath;

////////////////////////////////////////////////////////////////////////////////
/// @brief use actions
////////////////////////////////////////////////////////////////////////////////

        bool _useActions;

////////////////////////////////////////////////////////////////////////////////
/// @brief enables development mode
////////////////////////////////////////////////////////////////////////////////

        bool _developmentMode;

////////////////////////////////////////////////////////////////////////////////
/// @brief enables frontend development mode
////////////////////////////////////////////////////////////////////////////////

        bool _frontendDevelopmentMode;

////////////////////////////////////////////////////////////////////////////////
/// @brief perform a database upgrade
///
/// @CMDOPT{\--upgrade}
///
/// Specifying this option will make the server perform a database upgrade at
/// start. A database upgrade will first compare the version number stored in
/// the file VERSION in the database directory with the current server version.
///
/// If the two version numbers match, the server will start normally.
///
/// If the version number found in the database directory is higher than the
/// version number the server is running, the server expects this is an
/// unintentional downgrade and will warn about this. It will however start
/// normally. Using the server in these conditions is however not recommended
/// nor supported.
///
/// If the version number found in the database directory is lower than the
/// version number the server is running, the server will check whether there
/// are any upgrade tasks to perform. It will then execute all required upgrade
/// tasks and print their statuses. If one of the upgrade tasks fails, the
/// server will exit and refuse to start.
/// Re-starting the server with the upgrade option will then again trigger the
/// upgrade check and execution until the problem is fixed. If all tasks are
/// finished, the server will start normally.
///
/// Whether or not this option is specified, the server will always perform a
/// version check on startup. Running the server with a non-matching version
/// number in the VERSION file will make the server refuse to start.
////////////////////////////////////////////////////////////////////////////////

        bool _performUpgrade;

////////////////////////////////////////////////////////////////////////////////
/// @brief perform a database upgrade
////////////////////////////////////////////////////////////////////////////////

        bool _skipUpgrade;

////////////////////////////////////////////////////////////////////////////////
/// @brief JavaScript garbage collection interval (each x requests)
///
/// @CMDOPT{\--javascript.gc-interval @CA{interval}}
///
/// Specifies the interval (approximately in number of requests) that the
/// garbage collection for JavaScript objects will be run in each thread.
////////////////////////////////////////////////////////////////////////////////

        uint64_t _gcInterval;

////////////////////////////////////////////////////////////////////////////////
/// @brief JavaScript garbage collection frequency (each x seconds)
///
/// @CMDOPT{\--javascript.gc-frequency @CA{frequency}}
///
/// Specifies the frequency (in seconds) for the automatic garbage collection of
/// JavaScript objects. This setting is useful to have the garbage collection
/// still work in periods with no or little numbers of requests.
////////////////////////////////////////////////////////////////////////////////

        double _gcFrequency;

////////////////////////////////////////////////////////////////////////////////
/// @brief optional arguments to pass to v8
///
/// @CMDOPT{\--javascript.v8-options @CA{options}}
///
/// Optional arguments to pass to the V8 Javascript engine. The V8 engine will
/// run with default settings unless explicit options are specified using this
/// option. The options passed will be forwarded to the V8 engine which will
/// parse them on its own. Passing invalid options may result in an error being
/// printed on stderr and the option being ignored.
///
/// Options need to be passed in one string, with V8 option names being prefixed
/// with double dashes. Multiple options need to be separated by whitespace.
/// To get a list of all available V8 options, you can use
/// the value @LIT{"--help"} as follows:
/// @code
/// --javascript.v8-options "--help"
/// @endcode
///
/// Another example of specific V8 options being set at startup:
/// @code
/// --javascript.v8-options "--harmony --log"
/// @endcode
///
/// Names and features or usable options depend on the version of V8 being used,
/// and might change in the future if a different version of V8 is being used
/// in ArangoDB. Not all options offered by V8 might be sensible to use in the
/// context of ArangoDB. Use the specific options only if you are sure that
/// they are not harmful for the regular database operation.
////////////////////////////////////////////////////////////////////////////////

        string _v8Options;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 startup loader
////////////////////////////////////////////////////////////////////////////////

        JSLoader _startupLoader;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 action loader
////////////////////////////////////////////////////////////////////////////////

        JSLoader _actionLoader;

////////////////////////////////////////////////////////////////////////////////
/// @brief system database
////////////////////////////////////////////////////////////////////////////////

        struct TRI_vocbase_s* _vocbase;
        
////////////////////////////////////////////////////////////////////////////////
/// @brief number of instances to create
////////////////////////////////////////////////////////////////////////////////

        size_t _nrInstances;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 contexts
////////////////////////////////////////////////////////////////////////////////

        V8Context** _contexts;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 contexts queue lock
////////////////////////////////////////////////////////////////////////////////

        basics::ConditionVariable _contextCondition;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 free contexts
////////////////////////////////////////////////////////////////////////////////

        std::vector<V8Context*> _freeContexts;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 free contexts
////////////////////////////////////////////////////////////////////////////////

        std::vector<V8Context*> _dirtyContexts;

////////////////////////////////////////////////////////////////////////////////
/// @brief V8 busy contexts
////////////////////////////////////////////////////////////////////////////////

        std::set<V8Context*> _busyContexts;

////////////////////////////////////////////////////////////////////////////////
/// @brief shutdown in progress
////////////////////////////////////////////////////////////////////////////////

        volatile sig_atomic_t _stopping;

////////////////////////////////////////////////////////////////////////////////
/// @brief garbage collection thread
////////////////////////////////////////////////////////////////////////////////

        basics::Thread* _gcThread;
    };
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

#endif

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
