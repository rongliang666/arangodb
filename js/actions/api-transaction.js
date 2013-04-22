////////////////////////////////////////////////////////////////////////////////
/// @brief transaction actions
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2012 triagens GmbH, Cologne, Germany
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
/// @author Jan Steemann
/// @author Copyright 2012, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

var arangodb = require("org/arangodb");
var actions = require("org/arangodb/actions");

// -----------------------------------------------------------------------------
// --SECTION--                                                 private functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoAPI
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief execute a server-side transaction
///
/// @RESTHEADER{POST /_api/transaction,executes a transaction}
///
/// @REST{POST /_api/transaction}
///
/// The transaction description must be passed in the body of the POST request.
///
/// The following attributes must be specified inside the JSON object:
///
/// - @LIT{collections}: contains the list of collections to be used in the
///   transaction (mandatory). @LIT{collections} must be a JSON array that can
///   have the optional sub-attributes @LIT{read} and @LIT{write}. @LIT{read}
///   and @LIT{write} must each be either lists of collections names or strings
///   with a single collection name. 
///
/// - @LIT{action}: the actual transaction operations to be executed, in the
///   form of stringified Javascript code. The code will be executed on server
///   side, with late binding. It is thus critical that the code specified in
///   @LIT{action} properly sets up all the variables it needs. 
///   If the code specified in @LIT{action} ends with a return statement, the
///   value returned will also be returned by the REST API in the @LIT{result}
///   attribute if the transaction committed successfully.
///
/// The following optional attributes may also be specified in the request:
///
/// - @LIT{waitForSync}: an optional boolean flag that, if set, will force the 
///   transaction to write all data to disk before returning.
///
/// - @LIT{lockTimeout}: an optional numeric value that can be used to set a
///   timeout for waiting on collection locks. If not specified, a default 
///   value will be used. Setting @LIT{lockTimeout} to @LIT{0} will make ArangoDB 
///   not time out waiting for a lock.
///
/// - @LIT{params}: optional arguments passed to @LIT{action}.
///
/// If the transaction is fully executed and committed on the server, 
/// @LIT{HTTP 200} will be returned. Additionally, the return value of the 
/// code defined in @LIT{action} will be returned in the @LIT{result} attribute.
/// 
/// For successfully committed transactions, the returned JSON object has the 
/// following properties:
///
/// - @LIT{error}: boolean flag to indicate if an error occurred (@LIT{false}
///   in this case)
///
/// - @LIT{code}: the HTTP status code
///
/// - @LIT{result}: the return value of the transaction
///
/// If the transaction specification is either missing or malformed, the server
/// will respond with @LIT{HTTP 400}.
///
/// The body of the response will then contain a JSON object with additional error
/// details. The object has the following attributes:
///
/// - @LIT{error}: boolean flag to indicate that an error occurred (@LIT{true} in this case)
///
/// - @LIT{code}: the HTTP status code
///
/// - @LIT{errorNum}: the server error number
///
/// - @LIT{errorMessage}: a descriptive error message
///
/// If a transaction fails to commit, either by an exception thrown in the 
/// @LIT{action} code, or by an internal error, the server will respond with 
/// an error. 
///
/// Exceptions thrown by users will make the server respond with a return code of 
/// @LIT{HTTP 500}. Any other errors will be returned with any of the return codes
/// @LIT{HTTP 400}, @LIT{HTTP 409}, or @LIT{HTTP 500}.
///
/// @EXAMPLES
///
/// Executing a transaction on a single collection:
///
/// @verbinclude api-transaction-single
///
/// Executing a transaction using multiple collections:
///
/// @verbinclude api-transaction-multi
///
/// Aborting a transaction due to an internal error:
///
/// @verbinclude api-transaction-abort-internal
///
/// Aborting a transaction by throwing an exception:
///
/// @verbinclude api-transaction-abort
////////////////////////////////////////////////////////////////////////////////

function POST_api_transaction(req, res) {
  var json = actions.getJsonBody(req, res);

  if (json === undefined) {
    actions.resultBad(req, res, arangodb.ERROR_HTTP_BAD_PARAMETER);
    return;
  }

  var result = TRANSACTION(json);

  actions.resultOk(req, res, actions.HTTP_OK, { "result" : result });
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                       initialiser
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief gateway 
////////////////////////////////////////////////////////////////////////////////

actions.defineHttp({
  url : "_api/transaction",
  context : "api",

  callback : function (req, res) {
    try {
      switch (req.requestType) {
        case (actions.POST) : 
          POST_api_transaction(req, res); 
          break;

        default:
          actions.resultUnsupported(req, res);
      }
    }
    catch (err) {
      actions.resultException(req, res, err);
    }
  }
});

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// @addtogroup\\|// --SECTION--\\|/// @page\\|/// @}\\)"
// End:
