String _CSS = "<style> html { height: -webkit-fill-available; } .content {max-width: 500px;margin: auto;}h1{ font-size: 30px; color: #fff; text-transform: uppercase; font-weight: 300; text-align: center; margin-bottom: 15px; } p{ font-size: 0.6em; } body{ min-height: 100vh; min-height: -webkit-fill-available; margin:5px; }header{ min-height:50px; }footer{ min-height:50px; } table{ width:100%; table-layout: auto; } .tbl-header{ background-color: rgba(255,255,255,0.3); } .tbl-content{ height:300px; overflow-x:auto; margin-top: 0px; border: 1px solid rgba(255,255,255,0.3); } th{ padding: 10px 5px; text-align: center; font-weight: 500; font-size: 10px; color: #fff; text-transform: uppercase; } td{ padding: 5px; text-align: center; vertical-align:middle; font-weight: 300; font-size: 10px; color: #fff; border-bottom: solid 1px rgba(255,255,255,0.1); } /* demo styles */ @import url(https://fonts.googleapis.com/css?family=Roboto:400,500,300,700); body{ display:flex; flex-direction:column; background: -webkit-linear-gradient(left, #25c481, #25b7c4); background: linear-gradient(to right, #25c481, #25b7c4); font-family: 'Roboto', sans-serif; } section{ margin: 50px; } .button {padding: 10px 10px;font-size: 18px;text-align: center;outline: none; color: #fff;background-color: #0f8b8d;border: none; margin-right: 4px; border-radius: 5px;-webkit-touch-callout: none;-webkit-user-select: none;-khtml-user-select: none;-moz-user-select: none;-ms-user-select: none;user-select: none;-webkit-tap-highlight-color: rgba(0,0,0,0);}.button-ontbl {padding: 10px 10px;font-size: 10px;text-align: center;outline: none; color: #fff;background-color: #0f8b8d;border: none;border-radius: 5px;-webkit-touch-callout: none;-webkit-user-select: none;-khtml-user-select: none;-moz-user-select: none;-ms-user-select: none;user-select: none;-webkit-tap-highlight-color: rgba(0,0,0,0);}nav { background: -webkit-linear-gradient(right, #25c481, #25b7c4); background: linear-gradient(to left, #25c481, #25b7c4); font-family: 'Roboto', sans-serif;  color: #fff; display: block; font-size: 1.3em; padding: 1em; } footer {margin-top:auto; text-align: center; padding: 3px; background: -webkit-linear-gradient(right, #25c481, #25b7c4); background: linear-gradient(to left, #25c481, #25b7c4); font-family: 'Roboto', sans-serif; color: white;}</style>";

String _Head = "<html><head><meta name='viewport' content='initial-scale=1.0, width=device-width'>";

String _tempHTML = _Head + _CSS + "</head><body>"
                   "<nav><strong>Deauther Fluxion | Scan Network</strong><br><p>" + versi + "</p></nav><br>"
                   "<div><form style='display:inline-block;' method='post' action='/?deauth={deauth}'>"
                   "<button id='button' class='button' button style='display:inline-block;'{disabled}>{deauth_button}</button></form>"
                   "<form style='display:inline-block;' method='post' action='/?hotspot={hotspot}'>"
                   "<button id='button' class='button' button style='display:inline-block;'{disabled}>{hotspot_button}</button></form>"
                   "</div></br><table class='tbl-header'><tr><th>SSID</th><th>BSSID | Mac Address</th><th>PWR</th><th>CH</th><th>Select</th></tr>";

String _tempHTML2 = _Head + _CSS + "</head><body>";

String _ClearPassword = "<html><head><script> setTimeout(function(){window.location.href = '/admin';}, 3000); </script><meta name='viewport' content='initial-scale=1.0, width=device-width'>" + _CSS + "</head><body>";
