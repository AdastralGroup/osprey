extends Control
var of_theme = load("res://themes/of_theme.tres")
var of_bg = load("res://assets/of-bg.png")
var of_logo = load("res://assets/adastral-of.png")

var tf2c_theme = load("res://themes/of_theme.tres")
var tf2c_bg = load("res://assets/tf2c-bg.png")
var tf2c_logo = load("res://assets/adastral-tf2c.png")

var pf2_theme = load("res://themes/pf2_theme.tres")
var pf2_bg = load("res://assets/pf2-bg.png")
var pf2_logo = load("res://assets/adastral-pf2.png")

var lf_theme = load("res://themes/of_theme.tres")
var lf_bg = load("res://assets/of-bg.png")
var lf_logo = load("res://assets/adastral-tf2c.png")

var default_theme = load("res://themes/main.tres")
var default_bg = load("res://assets/bg.png")
var default_logo = load("res://assets/adastral.png")

signal change_to(game)
func _ready():
	theme = default_theme
	$Background.texture = default_bg
	$Background.rect_position = Vector2(0,64)
	#$Background.rect_size = Vector2(610,381)
	$GameText.text = "Welcome to Adastral. Choose a game!"
	#$GameText5.rect_position = Vector2(41,2)
	#$GameText5.rect_size = Vector2(111,45)
	$AdastralLogo.texture = default_logo
	
func _on_TextureButton_pressed():
	theme = of_theme
	$Background.texture = of_bg
	$Background.rect_position = Vector2(-220,-140)
	$Background.rect_size = Vector2(1000,600)
	$GameText.text = "You've chosen Open Fortress..."
	$AdastralLogo.texture = of_logo
	emit_signal("change_to","of")

func _on_TextureButton2_pressed():
	theme = default_theme
	$Background.texture = tf2c_bg
	$Background.rect_position = Vector2(0,-72)
	$Background.rect_size = Vector2(768,500)
	$GameText.text = "You've chosen TF2 Classic..."
	$AdastralLogo.texture = tf2c_logo
	$TextureRect3.material.set_shader_param("color",Color("#aa401400"))
	emit_signal("change_to","tf2c")

func _on_PF2Button_pressed():
	theme = default_theme
	$Background.texture = pf2_bg
	$Background.rect_position = Vector2(0,55)
	$Background.rect_size = Vector2(768,500)
	$GameText.text = "You've chosen Pre-Fortress 2..."
	
	$AdastralLogo.texture = pf2_logo
	$TextureRect3.material.set_shader_param("color",Color("#aab63538"))
	emit_signal("change_to","pf2")


func _on_Button4_pressed():
	_ready()





