extends VBoxContainer


# Declare member variables here. Examples:
# var a = 2
# var b = "text"

var from = null;
# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.



func clear():
	set_color(Color.WHITE)


func set_color(color):
	$TF2CButton.modulate = color
	$OFButton.modulate = color
	$PF2Button.modulate = color
	$LFButton.modulate = color
	$CF2Button.modulate = color
# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass



func _on_Main_change_to(game):
	print("called")
	clear()
	match game:
		"tf2c":
			set_color(Color("e8dbaf"))
			#$TF2CButton.modulate = Color("402311")
			return
		"of":
			set_color(Color("eee1cf"))
			return
		"pf2":
			#$PF2Button.modulate = Color("402311
			set_color(Color("e6e6dc"))
			return
		"lf":
			return
		"cf2":
			return
		"adastral":
			return
