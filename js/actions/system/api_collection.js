////////////////////////////////////////////////////////////////////////////////
/// @brief querying and managing collections
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
/// @author Achim Brandt
/// @author Copyright 2012, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

var actions = require("actions");
var API = "_api/";

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup AvocadoAPI
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief returns a collection
///
/// @REST{GET /_api/collection/@FA{collection-identifier}}
///
/// The result is an objects describing the collection with the following
/// attributes:
///
/// @LIT{id}: The identifier of the collection.
///
/// @LIT{name}: The name of the collection.
///
/// @LIT{status}: The status of the collection as number.
///
/// - 1: new born collection
/// - 2: unloaded
/// - 3: loaded
/// - 5: deleted
/// - 6: in the process of being unloaded
///
/// Every other status indicates a corrupted collection.
///
/// If the @FA{collection-identifier} is missing, then a @LIT{HTTP 400} is
/// returned.  If the @FA{collection-identifier} is unknown, then a @LIT{HTTP
/// 404} is returned.
///
/// It is possible to specify a name instead of an identifier.  In this case the
/// response will contain a field "Location" which contains the correct
/// location.
///
/// @REST{GET /_api/collection/@FA{collection-identifier}/count}
///
/// In addition to the above, the result also contains the number of documents.
/// Note that this will always load the collection into memory.
///
/// @LIT{count}: The number of documents inside the collection.
///
/// @REST{GET /_api/collection/@FA{collection-identifier}/figures}
///
/// In addition to the above, the result also contains the number of documents
/// and additional statistical information about the collection.  Note that this
/// will always load the collection into memory.
///
/// @LIT{count}: The number of documents inside the collection.
///
/// @LIT{figures}:
///
/// @EXAMPLES
///
/// Using an identifier:
///
/// @verbinclude api-collection-get-collection-identifier
///
/// Using a name:
///
/// @verbinclude api-collection-get-collection-name
///
/// Using an identifier and requesting the number of the collection:
///
/// @verbinclude api-collection-get-collection-number
///
/// Using an identifier and requesting the figures of the collection:
///
/// @verbinclude api-collection-get-collection-figures
////////////////////////////////////////////////////////////////////////////////

function GET_api_collection (req, res) {
  if (req.suffix.length == 0) {
    actions.resultBad(req, res, actions.ERROR_HTTP_BAD_PARAMETER,
                      "expect GET /" + API + "collection/<collection-identifer>")
  }
  else {
    var name = req.suffix[0];
    var id = parseInt(name) || name;
    var collection = db._collection(id);
    
    if (collection == null) {
      actions.collectionNotFound(req, res, name);
    }
    else {

      // .............................................................................
      // /_api/collection/<identifier>
      // .............................................................................

      if (req.suffix.length == 1) {
        var result = {};
        var headers = {};
      
        result.id = collection._id;
        result.name = collection.name();
        result.status = collection.status();

        headers.location = "/" + API + "collection/" + collection._id;
      
        actions.resultOk(req, res, actions.HTTP_OK, result, headers);
      }

      else if (req.suffix.length == 2) {
        var sub = req.suffix[1];

        // .............................................................................
        // /_api/collection/<identifier>/figures
        // .............................................................................

        if (sub == "figures") {
          var result = {};
          var headers = {};
      
          result.id = collection._id;
          result.name = collection.name();
          result.status = collection.status();
          result.count = collection.count();
          result.figures = collection.figures();

          headers.location = "/" + API + "collection/" + collection._id + "/figures";
      
          actions.resultOk(req, res, actions.HTTP_OK, result, headers);
        }

        // .............................................................................
        // /_api/collection/<identifier>/count
        // .............................................................................

        else if (sub == "count") {
          var result = {};
          var headers = {};
      
          result.id = collection._id;
          result.name = collection.name();
          result.count = collection.count();
          result.status = collection.status();

          headers.location = "/" + API + "collection/" + collection._id + "/count";
      
          actions.resultOk(req, res, actions.HTTP_OK, result, headers);
        }

        else {
          actions.resultNotFound(req, res, "expecting one of the sub-method 'count', 'figures'");
        }
      }
      else {
        actions.resultBad(req, res, actions.ERROR_HTTP_BAD_PARAMETER,
                          "expect GET /" + API + "collection/<collection-identifer>/<method>")
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief reads or creates a collection
////////////////////////////////////////////////////////////////////////////////

actions.defineHttp({
  url : API + "collection",
  context : "api",

  callback : function (req, res) {
    if (req.requestType == actions.GET) {
      GET_api_collection(req, res);
    }
    /*
    else if (req.requestType == actions.POST) {
      POST_api_database_collection(req, res);
    }
    */
    else {
      actions.resultUnsupported(req, res);
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
