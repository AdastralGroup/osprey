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

func set_title_text(text):
	$GameText.bbcode_text = "You're managing [b]%s[/b]" % text



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
#	var tween = get_tree().create_tween()
#	tween.tween_property($Camera2D,"zoom",Vector2(100,100),4).set_ease(Tween.EASE_IN).set_trans(Tween.TRANS_BOUNCE)
#	tween.tween_property($Camera2D,"zoom",Vector2(1,1),4).set_ease(Tween.EASE_OUT).set_trans(Tween.TRANS_BOUNCE)
#	yield(get_tree().create_timer(4),"timeout")
	theme = of_theme
	$Background.texture = of_bg
	#$Background.rect_position = Vector2(-220,-140)
	#$Background.rect_size = Vector2(1000,600)
	set_title_text("Open Fortress")
	$AdastralLogo.texture = of_logo
	emit_signal("change_to","of")

func _on_TextureButton2_pressed():
	theme = default_theme
	$Background.texture = tf2c_bg
	$Background.rect_position = Vector2(0,-72)
	$Background.rect_size = Vector2(768,500)
	set_title_text("TF2 Classic")
	$AdastralLogo.texture = tf2c_logo
	emit_signal("change_to","tf2c")

func _on_PF2Button_pressed():
	theme = pf2_theme
	$Background.texture = pf2_bg
	$Background.rect_position = Vector2(0,24)
	$Background.rect_size = Vector2(808,456)
	set_title_text("Pre-Fortress 2")
	$AdastralLogo.texture = pf2_logo
	emit_signal("change_to","pf2")


func _on_Button4_pressed():
	emit_signal("change_to","adastral")
	_ready()





