extends Control
var of_theme = load("res://of_theme.tres")
var of_font = load("res://of_normal_font.tres")
var of_bg = load("res://ofbg.png")
var of_logo = load("res://adastral-of.png")

var tf2c_theme = load("res://of_theme.tres")
var tf2c_font = load("res://tf2c_normal_font.tres")
var tf2c_bg = load("res://ofbg.png")
var tf2c_logo = load("res://adastral-tf2c.png")


var default_theme = load("res://main.tres")
var default_font = load("res://font.tres")
var default_bg = load("res://bg.png")
var default_logo = load("res://adastral.png")

func _ready():
	theme = default_theme
	#getallnodes(self,default_font)
	$TextureRect2.texture = default_bg
	$TextureRect2.rect_position = Vector2(0,0)
	$TextureRect2.rect_size = Vector2(610,381)
	$Label.text = "This is Adastral. Click on a game!"
	#$Label5.rect_position = Vector2(41,2)
	#$Label5.rect_size = Vector2(111,45)
	$TextureRect.texture = default_logo
	
func _on_TextureButton_pressed():
	theme = of_theme
	#getallnodes(self,of_font)
	$TextureRect2.texture = of_bg
	$TextureRect2.rect_position = Vector2(-220,-140)
	$TextureRect2.rect_size = Vector2(1000,600)
	$Label.text = "You're installing Open Fortress..."
	#$Label5.rect_position = Vector2(41,-4)
	$TextureRect.texture = of_logo

func getallnodes(node,font):
	for N in node.get_children():
		if N.get_child_count() > 0:
			print("["+N.get_name()+"]")
			getallnodes(N,font)
		else:
			var z = font.duplicate()
			var x = N.get_font("font")
			if x.get_class() == "BitmapFont":
				return
			z.size = x.size
			N.add_font_override("font", z)
			print("font size is " + str(x.size))
			print("- "+N.get_name())


func _on_Button4_pressed():
	_ready()


func _on_TextureButton2_pressed():
	theme = default_theme
	#getallnodes(self,tf2c_font)
	$TextureRect2.texture = tf2c_bg
	$TextureRect2.rect_position = Vector2(-220,-140)
	$TextureRect2.rect_size = Vector2(1000,600)
	$Label.text = "You're installing TF2 Classic..."
	#$Label5.rect_position = Vector2(38,-4)
	$TextureRect.texture = tf2c_logo
