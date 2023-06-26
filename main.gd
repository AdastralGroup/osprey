extends Control
var of_theme = load("res://themes/of_theme.tres")
var of_bg = load("res://assets/of-bg.jpg")
var of_logo = load("res://assets/adastral-of.png")
var of_word = load("res://assets/of_wordmark_big.png")

var tf2c_theme = load("res://themes/tf2c_theme.tres")
var tf2c_bg = load("res://assets/tf2c-bg.jpg")
var tf2c_logo = load("res://assets/adastral-tf2c.png")
var tf2c_word = load("res://assets/tf2c_wordmark_big.png")

var pf2_theme = load("res://themes/pf2_theme.tres")
var pf2_bg = load("res://assets/pf2-bg.png")
var pf2_logo = load("res://assets/adastral-pf2.png")
var pf2_word = load("res://assets/pf2_wordmark_big.png")

#var lf_theme = load("res://themes/of_theme.tres")
#var lf_bg = load("res://assets/of-bg.png")
#var lf_logo = load("res://assets/adastral-tf2c.png")

var default_theme = load("res://themes/main.tres")
var default_bg = load("res://assets/bg.png")
var default_logo = load("res://assets/adastral.svg")



signal change_to(game)

func set_title_text(text):
	$GameText.text = "You've selected [b]%s[/b]" % text

func set_button_colours(colour):
	$TopPanel/AboutButton.modulate = colour
	$TopPanel/AdvancedButton.modulate = colour
	$TopPanel/HomeButton.modulate = colour
	$TopPanel/DownloadTab.modulate = colour


func _ready():
	var z = JSON.stringify(pf2_theme)
	print(z)
	$Wordmark.hide()
	$TextureRect2.show()
	theme = default_theme
	$Background.texture = default_bg
	$Background.position = Vector2(0,0)
	$Background.size = Vector2(770,352)
	$GameText.text = "[b]Welcome to Adastral![/b] Select a game to install."
	#$GameText5.rect_position = Vector2(41,2)
	#$GameText5.rect_size = Vector2(111,45)
	set_button_colours(Color("f8f3ee"))
	$TopPanel/AdastralLogo.texture = default_logo
	

func _on_OFButton_pressed():
#	var tween = get_tree().create_tween()
#	tween.tween_property($Camera2D,"zoom",Vector2(100,100),4).set_ease(Tween.EASE_IN).set_trans(Tween.TRANS_BOUNCE)
#	tween.tween_property($Camera2D,"zoom",Vector2(1,1),4).set_ease(Tween.EASE_OUT).set_trans(Tween.TRANS_BOUNCE)
#	yield(get_tree().create_timer(4),"timeout")
	$Wordmark.show()
	$TextureRect2.hide()
	theme = of_theme
	$Background.texture = of_bg
	$Background.position = Vector2(65,68)
	$Background.size = Vector2(1135,397)
	$Wordmark.texture = of_word
	$Wordmark.position = Vector2(77,181)
	$Wordmark.size = Vector2(338,64)
	set_title_text("Open Fortress")
	$TopPanel/AdastralLogo.texture = of_logo
	set_button_colours(Color("eee1cf"))
	emit_signal("change_to","of")



func _on_TF2CButton_pressed():
	$Wordmark.show()
	$TextureRect2.hide()
	theme = tf2c_theme
	$Background.texture = tf2c_bg
	$Background.position = Vector2(-24,36) # bg positioning, we won't need this with proper assets
	$Background.size = Vector2(876,332)
	$Wordmark.texture = tf2c_word
	$Wordmark.position = Vector2(77,139)
	$Wordmark.size = Vector2(338,106)
	set_title_text("TF2 Classic")
	$TopPanel/AdastralLogo.texture = tf2c_logo
	set_button_colours(Color("e7daad"))
	emit_signal("change_to","tf2c") # to change the colour buttons in a different file

func _on_PF2Button_pressed():
	$Wordmark.show()
	$TextureRect2.hide()
	theme = pf2_theme
	$Background.texture = pf2_bg
	$Background.position = Vector2(-24,36)
	$Background.size = Vector2(876,332)
	$Wordmark.texture = pf2_word
	$Wordmark.position = Vector2(77,139)
	$Wordmark.size = Vector2(338,106)
	set_title_text("Pre-Fortress 2")
	$TopPanel/AdastralLogo.texture = pf2_logo
	set_button_colours(Color("e6e6dc"))
	emit_signal("change_to","pf2")


func _on_Button4_pressed():
	pass



func _on_AboutButton_pressed():
	get_window().set_size(Vector2(770, 350))


func _on_HomeButton_pressed():
	print("clicked")
	emit_signal("change_to","adastral")
	_ready()
