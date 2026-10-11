#!/bin/tclsh
# Heizkalender-Addon: Update-Check-CGI
# Zielplattform: CCU3, Tcl 8.2.3
#
# Protokoll (CCU WebUI):
#   ?cmd=check_version&version=<installiert>  -> neueste Version oder "n/a"
#   ?cmd=download&version=<installiert>       -> Redirect zur Release-Download-URL

# GitHub-Koordinaten
set gh_owner  "tatzel"
set gh_repo   "Homematic-Heizungskalender"
set gh_api    "api.github.com"
set gh_asset  "heizkalender-addon.tar.gz"

# ---------------------------------------------------------------------------
# Query-Parameter auslesen
# ---------------------------------------------------------------------------
set cmd ""
set installed_version ""

set query_string ""
catch {set query_string $env(QUERY_STRING)}

foreach param [split $query_string "&"] {
    set eqpos [string first "=" $param]
    if {$eqpos < 0} { continue }
    set key [string range $param 0 [expr {$eqpos-1}]]
    set val [string range $param [expr {$eqpos+1}] end]
    if {[string equal $key "cmd"]}     { set cmd $val }
    if {[string equal $key "version"]} { set installed_version $val }
}

# ---------------------------------------------------------------------------
# GitHub Releases API abfragen via HTTP (kein https nötig, curl vorhanden)
# ---------------------------------------------------------------------------
proc fetch_latest_release {owner repo asset} {
    set url "https://api.github.com/repos/${owner}/${repo}/releases/latest"
    set tmpfile "/tmp/hk_update_check.json"
    set rc [catch {
        exec curl -s -m 10 -o $tmpfile $url
    }]
    if {$rc != 0} { return {} }
    if {![file exists $tmpfile]} { return {} }
    set fh [open $tmpfile r]
    set data [read $fh]
    close $fh
    file delete -force $tmpfile

    # tag_name extrahieren: "tag_name": "v1.2.3"
    set tag ""
    set tpos [string first "\"tag_name\"" $data]
    if {$tpos >= 0} {
        set colon [string first ":" $data $tpos]
        set q1    [string first "\"" $data [expr {$colon+1}]]
        set q2    [string first "\"" $data [expr {$q1+1}]]
        if {$q1 >= 0 && $q2 > $q1} {
            set tag [string range $data [expr {$q1+1}] [expr {$q2-1}]]
        }
    }

    # browser_download_url fuer das Asset extrahieren
    set dl_url ""
    set apos [string first "\"${asset}\"" $data]
    if {$apos >= 0} {
        set bpos [string first "\"browser_download_url\"" $data $apos]
        if {$bpos >= 0} {
            set colon [string first ":" $data $bpos]
            set q1    [string first "\"" $data [expr {$colon+1}]]
            set q2    [string first "\"" $data [expr {$q1+1}]]
            if {$q1 >= 0 && $q2 > $q1} {
                set dl_url [string range $data [expr {$q1+1}] [expr {$q2-1}]]
            }
        }
    }

    return [list tag $tag dl_url $dl_url]
}

proc lookup {lst key} {
    set idx [lsearch $lst $key]
    if {$idx >= 0} { return [lindex $lst [expr {$idx+1}]] }
    return ""
}

# ---------------------------------------------------------------------------
# Antwort
# ---------------------------------------------------------------------------
set v_check  "check_version"
set v_dl     "download"

if {[string equal $cmd $v_check]} {
    set release [fetch_latest_release $gh_owner $gh_repo $gh_asset]
    set tag [lookup $release "tag"]
    # Tag-Prefix "v" entfernen falls vorhanden
    if {[string index $tag 0] == "v"} { set tag [string range $tag 1 end] }
    puts "Content-Type: text/plain\n"
    if {[string equal $tag ""]} {
        puts "n/a"
    } else {
        puts $tag
    }
} elseif {[string equal $cmd $v_dl]} {
    set release [fetch_latest_release $gh_owner $gh_repo $gh_asset]
    set dl_url [lookup $release "dl_url"]
    puts "Content-Type: text/html\n"
    if {[string equal $dl_url ""]} {
        puts "<html><body>Kein Release gefunden.</body></html>"
    } else {
        puts "<html><head><meta http-equiv=\"refresh\" content=\"0;url=[string map {& &amp;} $dl_url]\"></head>"
        puts "<body>Weiterleitung zu <a href=\"[string map {& &amp;} $dl_url]\">$dl_url</a>...</body></html>"
    }
} else {
    puts "Content-Type: text/plain\n"
    puts "n/a"
}
