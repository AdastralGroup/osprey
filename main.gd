extends Control
var of_theme = load("res://themes/of_theme.tres")
var of_bg = load("res://assets/of-bg.png")
var of_logo = load("res://assets/adastral-of.png")

var tf2c_theme = load("res://themes/tf2c_theme.tres")
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
	$GameText.text = "You've selected [b]%s[/b]" % text



func _ready():
	$TextureRect2.show()
	theme = default_theme
	$Background.texture = default_bg
	$Background.position = Vector2(0,0)
	$Background.size = Vector2(770,352)
	$GameText.text = "[b]Welcome to Adastral![/b] Select a game to install."
	#$GameText5.rect_position = Vector2(41,2)
	#$GameText5.rect_size = Vector2(111,45)
	$TopPanel/AboutButton.modulate = Color("f8f3ee")
	$TopPanel/AdvancedButton.modulate = Color("f8f3ee")
	$TopPanel/AdastralLogo.texture = default_logo
	

func _on_OFButton_pressed():
#	var tween = get_tree().create_tween()
#	tween.tween_property($Camera2D,"zoom",Vector2(100,100),4).set_ease(Tween.EASE_IN).set_trans(Tween.TRANS_BOUNCE)
#	tween.tween_property($Camera2D,"zoom",Vector2(1,1),4).set_ease(Tween.EASE_OUT).set_trans(Tween.TRANS_BOUNCE)
#	yield(get_tree().create_timer(4),"timeout")
	$TextureRect2.hide()
	theme = of_theme
	$Background.texture = of_bg
	$Background.position = Vector2(0,24)
	$Background.size = Vector2(810,456)
	set_title_text("Open Fortress")
	$TopPanel/AdastralLogo.texture = of_logo
	$TopPanel/AboutButton.modulate = Color("eee1cf") ## this isn't flexible but we're doing it untill I figure out a better way
	$TopPanel/AdvancedButton.modulate = Color("eee1cf")
	emit_signal("change_to","of")



func _on_TF2CButton_pressed():
	$TextureRect2.hide()
	theme = tf2c_theme
	$Background.texture = tf2c_bg
	$Background.position = Vector2(-40,0) # bg positioning, we won't need this with proper assets
	$Background.size = Vector2(810,266)
	set_title_text("TF2 Classic")
	$TopPanel/AdastralLogo.texture = tf2c_logo
	$TopPanel/AboutButton.modulate = Color("e7daad") ## we can't set this in themes so we do it manually
	$TopPanel/AdvancedButton.modulate = Color("e7daad")
	emit_signal("change_to","tf2c") # to change the colour buttons in a different file

func _on_PF2Button_pressed():
	$TextureRect2.hide()
	theme = pf2_theme
	$Background.texture = pf2_bg
	$Background.position = Vector2(0,24)
	$Background.size = Vector2(810,458)
	set_title_text("Pre-Fortress 2")
	$TopPanel/AdastralLogo.texture = pf2_logo
	$TopPanel/AboutButton.modulate = Color("e6e6dc")
	$TopPanel/AdvancedButton.modulate = Color("e6e6dc")
	emit_signal("change_to","pf2")


func _on_Button4_pressed():
	print("clicked")
	emit_signal("change_to","adastral")
	_ready()



func _on_AboutButton_pressed():
	get_window().set_size(Vector2(770, 350))
