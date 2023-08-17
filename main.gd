extends Control

#var big_json='{"adastral":{"dark":"#000000","light":"#f8f3ee","main":"#005c9d","accent":"#b76300","lightfg":"#8d847b","bgpath":"res://assets/bg.png","logo":"res://assets/adastral.svg","wordmark":"res://assets/adastral.svg"},"open_fortress":{"dark":"#231f20","light":"#eee1cf","main":"#59416a","accent":"#d49c6b","lightfg":"#8d847b","bgpath":"res://assets/of-bg.jpg","logo":"res://assets/adastral-of.png","wordmark":"res://assets/of_wordmark_big.png"}}'
@onready var theme_json
@onready var pallete: Dictionary

signal change_to(game)


func set_button_colours(colour):
	$TopPanel/AboutButton.modulate = colour
	$TopPanel/AdvancedButton.modulate = colour
	$TopPanel/HomeButton.modulate = colour
	$TopPanel/DownloadTab.modulate = colour
	for x in $HBoxContainer.get_children():
		if x.get_class() == "TextureButton":
			x.modulate = colour


func _ready():
	var json_handler = JSON.new()
	var file = FileAccess.open("res://assets/themes.json", FileAccess.READ)
	theme_json = json_handler.parse_string(file.get_as_text())
	#theme = load("res://themes/pf2_theme.tres")
	apply_theme("adastral")
	do_thing()
	
	
func _on_Button4_pressed():
	pass

func _on_AboutButton_pressed():
	pass


func _on_HomeButton_pressed():
	apply_theme("adastral")

func set_stylebox_colour(stylebox,colour):
	stylebox.bg_color = colour

func apply_theme(theme_name):
	pallete = theme_json[theme_name]
	var base_theme = load("res://themes/base.tres") 
	if not pallete.has("secondary"):
		pallete["secondary"] = pallete["main"]
	if pallete["id"] == "adastral":
		$TextureRect2.show()
	else:
		$TextureRect2.hide()
	if not pallete.has("wordmark"):
		$Wordmark.hide()
	else:
		$Wordmark.texture = load(pallete["wordmark"])
		$Wordmark.show()
	$Panel2/Background.texture = load(pallete["bgpath"])
	$TopPanel/AdastralLogo.texture = load(pallete["star"])
	set_stylebox_colour(base_theme.get_stylebox("panel","TopPanel"),Color(pallete["dark"]))
	set_stylebox_colour(base_theme.get_stylebox("panel","SidePanel"),Color(pallete["secondary"]))
	set_stylebox_colour(base_theme.get_stylebox("panel","AccentPanel"),Color(pallete["main"]))
	set_stylebox_colour(base_theme.get_stylebox("normal","Button"),Color(pallete["light"]))
	set_stylebox_colour(base_theme.get_stylebox("normal","ImportantButton"),Color(pallete["accent"]))
	set_stylebox_colour(base_theme.get_stylebox("background","TopPanel"),Color(pallete["light"]))
	set_stylebox_colour(base_theme.get_stylebox("fill","ProgressBar"),Color(pallete["accent"]))
	set_stylebox_colour(base_theme.get_stylebox("panel","TopPanel"),Color(pallete["dark"]))
	base_theme.set_color("font_color","Button",Color(pallete["lightfg"]))
	base_theme.set_color("font_color","ImportantButton",Color(pallete["light"]))
	theme = base_theme
	set_button_colours(Color(pallete["light"]))
	
	

func add_new_sep():
	var new = VSeparator.new()
	new.size_flags_vertical = Control.SIZE_SHRINK_CENTER
	new.add_theme_stylebox_override("separator",StyleBoxEmpty.new())
	$HBoxContainer.add_child(new)

func do_thing():
	for x in theme_json.values():
		if x["id"] != "adastral":
			if $HBoxContainer.get_child_count() != 0:
				add_new_sep()
			var new = TextureButton.new()
			new.ignore_texture_size = true
			new.stretch_mode = TextureButton.STRETCH_SCALE
			new.custom_minimum_size= Vector2(42,42)
			new.size_flags_horizontal = Control.SIZE_SHRINK_CENTER
			new.size_flags_vertical = Control.SIZE_SHRINK_CENTER
			new.texture_normal = load(x["icon"])
			$HBoxContainer.add_child(new)
			new.pressed.connect(func(): apply_theme(x["id"]))
	
	
	
