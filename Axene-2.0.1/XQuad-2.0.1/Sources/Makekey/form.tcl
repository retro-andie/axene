#!/usr/bin/wish -f
#!/usr/local/bin/wish4.1 -f

proc write_key { file w append } {
    set key [exec makekey _form.txt | tail -1]
    exec rm _form.txt
    if { $append } then {
	set fd [open "~/.$file" a]
    } else {
	set fd [open "~/.$file" w]
    }
    puts $fd $key
    close $fd
    destroy $w
}

proc write_key2 { file w append } {
    set key [exec ./makekey _form.txt | tail -1]
    exec rm _form.txt
    if { $append } then {
	set fd [open "$file" a]
    } else {
	set fd [open "$file" w]
    }
    puts $fd $key
    close $fd
    destroy $w
}

proc ok { w } {
    global name society soft type
    global license_number date 

    set fd [open "_form.txt" w]
    
    if { $name != ""} then { 
	puts -nonewline $fd "NAME: "
	puts $fd $name
    }
    
    if { $society != ""} then { 
	puts -nonewline $fd "SOCIETY: "
	puts $fd $society
    }

    puts -nonewline $fd "SOFT: "
    puts $fd [string tolower $soft]
    
    puts -nonewline $fd "TYPE: "
    puts $fd [string tolower $type]

    if {[string tolower $type] == "commercial"} then {
	puts -nonewline $fd "LICENSENUMBER: "
	puts $fd $license_number
    }

    if { $date != ""} then { 
	puts -nonewline $fd "DATE: "
	puts $fd $date
    }

    close $fd

    if { [string tolower $type] == "server"} then {
	set file "axenekey"
    } else {
	set file [string tolower $soft]key
    }

    toplevel $w -class Dialog
    wm title $w "Axene Key Form"
    wm iconname $w "Axene Form"
    wm resizable $w no no

    frame $w.f_ask -relief flat -border 1
    pack $w.f_ask -side top -fill x
    
    label $w.f_ask.lb -bitmap questhead
    pack  $w.f_ask.lb -pady 2 -side left

    frame $w.f_ask.f_mes -relief flat -border 1
    pack $w.f_ask.f_mes -side left -fill x
    
    if { [string tolower $type] == "server"} then {
	label $w.f_ask.f_mes.m -text "Append key to file:" -relief flat
	set append 1
    } else {
	label $w.f_ask.f_mes.m -text "Copy key to file:" -relief flat
	set append 0
    }
    pack  $w.f_ask.f_mes.m -side top -padx 10 -expand yes -fill x
    label $w.f_ask.f_mes.l2 -text ~/.$file -relief flat
    pack  $w.f_ask.f_mes.l2 -side top -padx 10 -expand yes -fill x

    frame $w.f_button -relief sunken -border 1
    pack $w.f_button -side bottom -fill x -pady 2m
    button $w.f_button.b_ok -text "OK" -width 6 \
	    -command "write_key $file $w $append"
    button $w.f_button.b_cancel -text "Cancel" -width 6 \
	    -command "write_key2 $file $w $append"
    pack $w.f_button.b_ok $w.f_button.b_cancel  -side left -expand yes -pady 2
}

proc cancel {} {
    exit 0
}

proc sensitive {wname} {
    global w

    $w.f_$wname.e_$wname configure -state disabled -show " " 
    $w.f_$wname.l_$wname configure -foreground gray40
}

proc unsensitive {wname} {
    global w

    $w.f_$wname.e_$wname configure -state normal -show "" 
    $w.f_$wname.l_$wname configure -foreground black
}

proc create_text_field {w wname text variable} {
    frame $w.f_$wname -relief flat -border 1
    pack $w.f_$wname -side top -expand yes -fill x

    label $w.f_$wname.l_$wname -width 18 -anchor e -text $text
    entry $w.f_$wname.e_$wname -bd 1 -width 20 -textvariable $variable
    pack $w.f_$wname.l_$wname -side left 
    pack $w.f_$wname.e_$wname -side left -expand yes -fill x
}

proc create_window {} {
    global w name society soft type 
    global license_number 
    
    wm minsize . 50 50
    wm title . "Axene Key Form"
    wm iconname . "Axene Form"
    wm resizable . yes no
    
    create_text_field $w "userName" "User's Full Name : " name
    create_text_field $w "socityName" "Society : " society

    frame $w.f_soft -relief groove -border 2
    pack $w.f_soft -side top -expand yes -fill x -pady 2 -ipady 2
    
    frame $w.f_soft.f_softName -relief flat -border 1
    pack $w.f_soft.f_softName -side top -expand yes -fill x

    label $w.f_soft.f_softName.l_soft -width 18 -anchor e -text "Software : "
    menubutton $w.f_soft.f_softName.mb_soft -textvariable soft \
	    -menu $w.f_soft.f_softName.mb_soft.m_soft -relief raised \
	    -width 15 -bd 1
    pack $w.f_soft.f_softName.l_soft $w.f_soft.f_softName.mb_soft -side left
    
    menu $w.f_soft.f_softName.mb_soft.m_soft -tearoff 0
    $w.f_soft.f_softName.mb_soft.m_soft add radiobutton -label "Xclamation" \
	    -value "Xclamation" -variable soft 
    $w.f_soft.f_softName.mb_soft.m_soft add radiobutton -label "XQuad" \
	    -value "XQuad" -variable soft 
    $w.f_soft.f_softName.mb_soft.m_soft add radiobutton -label "XAllWrite" \
	    -value "XAllWrite" -variable soft 
    $w.f_soft.f_softName.mb_soft.m_soft add radiobutton -label "AxeneOffice" \
	    -value "AxeneOffice" -variable soft 

    frame $w.f_type -relief flat -border 1
    pack $w.f_type -side top -expand yes -fill x

    label $w.f_type.l_type -width 18 -anchor e -text "Key Type : "
    menubutton $w.f_type.mb_type -textvariable type \
	    -menu $w.f_type.mb_type.m_type -relief raised -width 15 -bd 1
    pack $w.f_type.l_type $w.f_type.mb_type -side left
    
    menu $w.f_type.mb_type.m_type -tearoff 0
    $w.f_type.mb_type.m_type add radiobutton -label "Free" \
	    -value "Free" -variable type -command \
	    { sensitive licenseNumber }
    $w.f_type.mb_type.m_type add radiobutton -label "Commercial" \
	    -value "Commercial" -variable type -command \
	    { unsensitive licenseNumber }

    create_text_field $w "licenseNumber" "Number of License : " \
	    license_number
    $w.f_licenseNumber.e_licenseNumber configure -state disabled -show " "
    $w.f_licenseNumber.l_licenseNumber configure -foreground gray40

    create_text_field $w "date" "Date : " date

    frame $w.buttons -relief sunken -border 1
    pack $w.buttons -side bottom -fill x -pady 2m
    button $w.buttons.ok -bd 1 -width 6 -command { ok .ok } -text OK
    button $w.buttons.cancel -bd 1 -width 6 -command cancel -text Cancel
    pack $w.buttons.ok $w.buttons.cancel -side left -expand yes -pady 2
}

#####################################
# Main program

set w ""

set myname [exec whoami]
set name [exec finger -p $myname | head -1 | cut -c32-]
set society [exec hostname -d | cut -d. -f1]
set soft "Xclamation"
set license_number 1
set type "Free"
set date [exec date +%m/%d/%Y]

create_window





