// Couchdb update handler which adds a 'created' field to the submited data
"updates": {
    "event": "function (oldDoc, req) {\n    var doc;\n\n    if (!oldDoc) {\n        doc = JSON.parse(req.body);\n        doc._id = req.uuid;\n        doc.created = Date.now();\n    }\n\n    return [doc, doc._id];\n}"
}
