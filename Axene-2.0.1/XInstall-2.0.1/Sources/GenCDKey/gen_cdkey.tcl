#!/usr/bin/wish -f
#!/usr/local/bin/wish4.1 -f

proc ok { w } {
    global distributor nb_key nb_license soft

    if { $soft == "Office" } then {
	set soft_id 0
    } elseif { $soft == "Xclamation" } then {
	set soft_id 1
    } elseif { $soft == "XQuad" } then {
	set soft_id 2
    } else {
	set soft_id 3
    }

    set fd [open "cdkey.db" a+]
    puts $fd "-------------------------------------------------------------"
    puts -nonewline $fd [exec date +%-d\ %B\ %Y]
    puts -nonewline $fd " - "
    puts -nonewline $fd $soft
    puts -nonewline $fd " #"
    puts -nonewline $fd $nb_license
    puts -nonewline $fd " - "
    puts $fd $distributor
    puts $fd "-------------------------------------------------------------"
    close $fd
   
    for { set i 0 } { $i < $nb_key } { set i [expr $i + 1 ] } {
	set key_ok 0
	while { $key_ok == 0 } {
	    set key [exec gen_cdkey -l $nb_license -s $soft_id]
	    if { [catch { exec grep "$key" cdkey.db }] } then {

		set fd [open "cdkey.db" a+]
		puts $fd $key
		close $fd

		puts $key
		set key_ok 1
	    }
	}
    }

    exit 0
}

proc cancel {} {
    exit 0
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
    global w distributor nb_key nb_license soft
    
    wm minsize . 50 50
    wm title . "Axene Key Generator for CD"
    wm iconname . "Gen CD-Key"
    wm resizable . yes no
 
    create_text_field $w "distributor" "Distributor Name : " distributor

    frame $w.f_softName -relief flat -border 1
    pack $w.f_softName -side top -expand yes -fill x

    label $w.f_softName.l_soft -width 18 -anchor e -text "Software : "
    menubutton $w.f_softName.mb_soft -textvariable soft \
	    -menu $w.f_softName.mb_soft.m_soft \
	    -relief raised -width 15 -bd 1
    pack $w.f_softName.l_soft $w.f_softName.mb_soft -side left
    
    menu $w.f_softName.mb_soft.m_soft
    $w.f_softName.mb_soft.m_soft add radiobutton -label "Axene Office" \
	    -value "Office" -variable soft
    $w.f_softName.mb_soft.m_soft add radiobutton -label "Xclamation" \
	    -value "Xclamation" -variable soft
    $w.f_softName.mb_soft.m_soft add radiobutton -label "XQuad" \
	    -value "XQuad" -variable soft
    $w.f_softName.mb_soft.m_soft add radiobutton -label "XAllWrite" \
	    -value "XAllWrite" -variable soft

    frame $w.f_nblic -relief flat -border 1
    pack $w.f_nblic -side top -expand yes -fill x

    label $w.f_nblic.l_nblic -width 18 -anchor e \
	    -text "Number of license : "
    entry $w.f_nblic.e_nblic -bd 1 -width 2 -textvariable nb_license
    pack $w.f_nblic.l_nblic $w.f_nblic.e_nblic -side left 

    frame $w.f_nbkey -relief flat -border 1
    pack $w.f_nbkey -side top -expand yes -fill x
    
    label $w.f_nbkey.l_nbkey -width 18 -anchor e -text "Number of Keys : "
    entry $w.f_nbkey.e_nbkey -bd 1 -width 3 -textvariable nb_key
    pack $w.f_nbkey.l_nbkey $w.f_nbkey.e_nbkey -side left 

    frame $w.buttons -relief sunken -border 1
    pack $w.buttons -side bottom -fill x -pady 2m
    button $w.buttons.ok -bd 1 -width 6 -command { ok .ok } -text OK
    button $w.buttons.cancel -bd 1 -width 6 -command cancel -text Cancel
    pack $w.buttons.ok $w.buttons.cancel -side left -expand yes -pady 2
}

#####################################
# Main program

set w ""

set distributor ""
set nb_key 10
set nb_license "0"
set soft "Office"

create_window