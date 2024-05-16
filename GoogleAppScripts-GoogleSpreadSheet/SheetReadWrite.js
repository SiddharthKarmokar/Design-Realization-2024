// Function to handle HTTP GET requests from the ESP32
//https://script.google.com/macros/s/AKfycbxVyTk7pLxGlQBxBENvaV1xzCHveYwOaOkUiAiHqZDWjlHkscD31e9FEEjRvpVdm_xQ4A/exec
function doGet(e) {
    var result = 'OK';
  
    var status = e.parameter.sts;
  
    var spreadsheet = SpreadsheetApp.openById('1XcaWF1ZmVIhu2F46quZPTarijfoM51r_2nEsdnPupEY');
    var sheet = spreadsheet.getActiveSheet();
  
    if(status == 'write')
    {
      var sensorData = e.parameter.data;
      sheet.getRange(sheet.getLastRow() + 1, 1).setValue(sensorData);
      result += " Written";
    }
    else if(status == 'read')
    {
      var readResult = '';
  
      var all_data = sheet.getRange(sheet.getLastRow(), 1).getValues();
  
      readResult += JSON.stringify(all_data.map(function(row){return row[0];}));
  
      return ContentService.createTextOutput(readResult).setMimeType(ContentService.MimeType.JSON);
    }
    return ContentService.createTextOutput(result);
  }