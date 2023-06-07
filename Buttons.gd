extends HBoxContainer


# Declare member variables here. Examples:
# var a = 2
# var b = "text"
var of = load("res://assets/of.png")
var of_mono = load("res://assets/of-mono.png")
var pf2 = load("res://assets/pf2.png")
var pf2_mono = load("res://assets/pf2-mono.png")
var tf2c = load("res://assets/tf2c.png")
var tf2c_mono = load("res://assets/tf2c-mono.png")
var lf = load("res://assets/tf2c.png")
var lf_mono = load("res://assets/lf-mono.png")

var from = null;
# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.



func clear():
	$TF2CButton.texture_normal = tf2c_mono
	$OFButton.texture_normal = of_mono
	$PF2Button.texture_normal = pf2_mono
	$LFButton.texture_normal = lf_mono
	set_color(Color.WHITE)


func set_color(color):
	$TF2CButton.modulate = color
	$OFButton.modulate = color
	$PF2Button.modulate = color
	$LFButton.modulate = color
# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass



func _on_Main_change_to(game):
	print("called")
	clear()
	match game:
		"tf2c":
			$TF2CButton.modulate = Color("402311")
			return
		"of":
			$OFButton.modulate = Color("59416a")
			return
		"pf2":
			$PF2Button.modulate = Color("402311")
			return
		"lf":
			$LFButton.texture_normal = lf
			return
		"adastral":
			return
