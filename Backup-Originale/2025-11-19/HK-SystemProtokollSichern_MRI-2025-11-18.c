!//Script um den Systemlog auf den USB Stick zu schreiben.

string vrp = "";
string pfad = "/media/usb1/";

!// Code wurde aus dem folgenden Thread übernommen:
!//     https://homematic-forum.de/forum/viewtopic.php?f=19&t=75602&p=847023#p735221

!// Ausgabe in eine Datei
string stdout;
string stderr;
string dateiname;
dateiname = pfad # "HK-Log_" # system.Date("%Y%m%d") # ".log";

!// Cleanup. Lösche so viele ateien, biss nur noch 100 übrig sind.


integer cnt = dom.GetHistoryDataCount();
integer iLastGroupIndex = 1;
string sCollectedNames = "";
string sCollectedValues = "";
string sCollectedDateTimes = "";
string sDatensatz;
integer rCount;
string sTotal="";

!// Letzten Datensatz ermitteln
string letzterGeschriebenerDatensatz=dom.GetObject(vrp+"HK-SystemProtokollSichern").State();
!// Wir starten im Modus Suchen
boolean modusSuchen=true;

!// Wenn wir keinen Suchstring haben, schreiben wir sofort
if (letzterGeschriebenerDatensatz==""){
  modusSuchen = false;
}

while (true){
  !// Wir	durchlaufen jetzt das Protokoll, entweder suchen wir oder wir schreiben
  foreach( sDatensatz, dom.GetHistoryData(0,cnt, &rCount ) ){
    integer iGroupIndex = sDatensatz.StrValueByIndex(";",0).ToInteger();
    string sDatapointId = sDatensatz.StrValueByIndex(";",1);
    string sRecordedValue = sDatensatz.StrValueByIndex(";",2);
    string sDateTime = sDatensatz.StrValueByIndex(";",3);
    string stmpDate = sDateTime.StrValueByIndex(" ",0);
    string stmpTime = sDateTime.StrValueByIndex(" ",1);
      
    if (modusSuchen) {
      if ((sDatensatz.StrValueByIndex(";",2)==letzterGeschriebenerDatensatz.StrValueByIndex(";",2)) && (sDatensatz.StrValueByIndex(";",3)==letzterGeschriebenerDatensatz.StrValueByIndex(";",3))){
        !// Datensatz gefunden, wir wechseln ion den Modus schreiben
        modusSuchen = false;
      }            
    } else {
      !// letzten Datensatz merken
      letzterGeschriebenerDatensatz = sDatensatz;
      
      string sDatapointName = "";
      object oHistDP = dom.GetObject( sDatapointId );
      if( oHistDP ) {
        object oDP = dom.GetObject( oHistDP.ArchiveDP() );
        if( oDP ) {
          sDatapointName = oDP.Name();
          boolean bSysVar = (oDP.IsTypeOf(OT_VARDP) || oDP.IsTypeOf(OT_ALARMDP));
          if( !bSysVar ) {
            object oCH = dom.GetObject( oDP.Channel() );
            if( oCH ) {
              sDatapointName = oCH.Name();
            }            
           }
        }
        
        if( iLastGroupIndex != iGroupIndex ) { 
          sCollectedNames = "";
          sCollectedValues = "";
          iLastGroupIndex = iGroupIndex;
        }
        
        string sRet = "";

        object to = dom.GetObject( oDP.ID());
        if( to ) {
          if( to.IsTypeOf( OT_VARDP ) || to.IsTypeOf( OT_ALARMDP ) ) {
            integer itoVT = to.ValueType();
            integer itoST = to.ValueSubType();
            boolean btoLogic  = ( (itoVT==ivtBinary)  && (itoST==istBool)    );
            boolean btoList   = ( (itoVT==ivtInteger) && (itoST==istEnum)    );
            boolean btoNumber = ( (itoVT==ivtFloat)   && (itoST==istGeneric) );
            boolean btoAlarm  = ( (itoVT==ivtBinary)  && (itoST==istAlarm)   );
            boolean btoString  = ( (itoVT==ivtString)  && (itoST==istChar8859));
    !// MRi: Wir geben den  Variablen Typ nicht aus
    !        if( (btoLogic || btoAlarm) && ((sRecordedValue == "0") || (sRecordedValue == "")) ) {
    !          sRet=sRet#to.ValueName0(); } else { sRet=sRet#to.ValueName1(); 
    !        }
            if( (btoList) && (sRecordedValue == "") ) { 
              sRet=sRet#web.webGetValueFromList(to.ValueList(),0); } else { sRet=sRet#web.webGetValueFromList(to.ValueList(),sRecordedValue.ToInteger()); 
            }
            if( btoNumber ) { 
              if (sRecordedValue == "") {
                real n = 0.0;
                sRet = sRet # n.ToString() # " (" # n.ToString(2);
              } else {
                sRet = sRet # sRecordedValue.ToString() # " (" # sRecordedValue.ToString(2);
              }
              if( to.ValueUnit() == "" ) { sRet = sRet # ")"; }
            }
            if (btoString) {
        !// MRi: Wir entfernen Zeilenschaltungen und Sicherheitshalber tauschen wir DoubleQuotes n single Quotes
              sRet = sRet # sRecordedValue;
              sRet = sRet.Replace("\"", "\'");
              sRet = sRet.Replace("\r\n", " ");
              sRet = sRet.Replace("\r", " ");
              sRet = sRet.Replace("\n", " ");
            }
            if( to.ValueUnit() != "" ) {
              sRet=sRet#" "#to.ValueUnit();
              if( btoNumber ) { sRet = sRet # ")"; }
            }
          } else {
            string tsShortKey = to.HSSID();
            string tsLongKey = to.HSSID();
            object toCH = dom.GetObject( to.Channel() );
            if( toCH ) { tsLongKey = toCH.ChnLabel()#"|"#tsLongKey; }
            
            boolean tbOptionList = ( (to.ValueType() == ivtInteger) && (to.ValueSubType() == istEnum) );
            boolean tbAction = ( to.ValueSubType() == istAction );
            boolean tbBinary = ( to.ValueType() == ivtBinary );
            boolean tbRead = (to.Operations() & OPERATION_READ);
            boolean tbEvent = (to.Operations() & OPERATION_EVENT);
            boolean tbWrite = (to.Operations() & OPERATION_WRITE);
            
            boolean bBinary = ( to.ValueTypeStr() == "Binary" );
            boolean bFloat = ( to.ValueTypeStr() == "Float" );
            boolean bSpecial = false;
                
            string sVUTmp = to.ValueUnit().ToString();
            string sSpace = " ";
            
            real fVal1 = 0.0;
            real fVal2 = 0.0;
            string sSpecial = "";
            string stmpSV;
            foreach(stmpSV,oDP.EnumSpecialIDs()) {
               fVal1 = oDP.GetSpecialValue(stmpSV);
               fVal2 = sRecordedValue.ToFloat();
               if( fVal1 == fVal2 ) {
                 bSpecial = true;
                 sSpecial = stmpSV;
                 sRecordedValue = "";
                 sVUTmp = "";
                 sSpace = "";
               }
            }
            
            if( tbBinary && (tbRead || tbAction) ) {
              if( sRecordedValue == "0" ) {
                tsShortKey = tsShortKey#"=FALSE";
                tsLongKey = tsLongKey#"=FALSE";
              } else {
                tsShortKey = tsShortKey#"=TRUE";
                tsLongKey = tsLongKey#"=TRUE";
              }
            }
            
            if( tbOptionList ) {
              tsShortKey = tsShortKey#"="#web.webGetValueFromList( to.ValueList(), sRecordedValue );
              tsLongKey = tsLongKey#"="#web.webGetValueFromList( to.ValueList(), sRecordedValue );
            }
            
            if( bSpecial ) {
              tsShortKey = tsShortKey#"="#sSpecial;
              tsLongKey = tsLongKey#"="#sSpecial;
            }        
            
            string sVTmp = tsLongKey ;
            if( !sVTmp.Length() ) { sVTmp = tsShortKey; }
            
            if( !bSpecial ) {
              if( sVUTmp == "100%" ) {
                sRecordedValue = sRecordedValue.ToFloat() * 100;
                sRecordedValue = sRecordedValue.ToString();
                sVUTmp = "%";
              }

              if( sVUTmp == "degree" ) { sVUTmp = "°"; }
              
              if( bBinary ) { sRecordedValue = ""; sSpace = ""; }
              
              if( bFloat ) { sRecordedValue = sRecordedValue.ToFloat().ToString(2); }
              
              if( (!bBinary) && (!bFloat) ) {
                if (((toCH.Label() != "HmIPW-DRAP") && (toCH.Label() != "HmIP-HAP")) || (sVUTmp == "°C") || (sVUTmp == "V")) {
                  sRecordedValue = sRecordedValue.ToInteger();
                  sRecordedValue = sRecordedValue.ToString(0);
                }
              }
              
              if( tbOptionList ) { sRecordedValue = ""; sVUTmp = ""; sSpace = ""; }
            }
            
            sVTmp = sVTmp#sSpace#sRecordedValue#sVUTmp;
            sRet = sVTmp;
          }
        }
        
        sRecordedValue = sRet;
        
        sCollectedNames = sDatapointName;
        sCollectedDateTimes = sDateTime;

        if( !sCollectedValues.Length() ) {
          sCollectedValues = sRecordedValue;
        } else {
          sCollectedValues = sCollectedValues#"\t"#sRecordedValue;
        }
      }
      string logLine=sCollectedDateTimes#"\t"#sCollectedNames#"\t"#sCollectedValues;
      ! logLine = logLine.ToLatin();
      !// Wir haben keinen Logger, wir sammeln die Daten
      sTotal = sTotal # logLine # "\n";
      
      !// Wenn wir die maximale Größe von 50000 erreicht haben müssen wir schreiben
      !// Bei einer größe über 120000 Bytes versagt echo, aber da wir einen Append nutzen,
      !// ist das kein Problem
      if (sTotal.Length()>50000){
        !// Zeilenschaltung entfernen
        sTotal = sTotal.RTrim();
        string cmd = "echo \"" # sTotal # "\" >> '" # dateiname # "'";
        system.Exec(cmd,&stdout,&stdeer);
        sTotal = "";
      }
    }  
  }
  if (modusSuchen){
    !// Wenn wir hierher kommen, dann haben wir keinen passenden datensatz gefunden
    !// Wir starten von vorne und schreiben jetzt
    modusSuchen = false;
  }else{
    !// Letzten Datensatz merken
    dom.GetObject(vrp+"HK-SystemProtokollSichern").State(letzterGeschriebenerDatensatz);
    break;
  }
}

!// Rest schreiben
if (sTotal.Length()>0){
	!// Zeilenschaltung entfernen
  sTotal = sTotal.RTrim();
  string cmd = "echo \"" # sTotal # "\" >> '" # dateiname # "'";
	system.Exec(cmd,&stdout,&stdeer);
	sTotal = "";
}

!// Nach erfolgreichem schreiben löschen wir das Systemlog
WriteLine("DONE");