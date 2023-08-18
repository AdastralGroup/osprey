extends Control

#var big_json='{"adastral":{"dark":"#000000","light":"#f8f3ee","main":"#005c9d","accent":"#b76300","lightfg":"#8d847b","bgpath":"res://assets/bg.png","logo":"res://assets/adastral.svg","wordmark":"res://assets/adastral.svg"},"open_fortress":{"dark":"#231f20","light":"#eee1cf","main":"#59416a","accent":"#d49c6b","lightfg":"#8d847b","bgpath":"res://assets/of-bg.jpg","logo":"res://assets/adastral-of.png","wordmark":"res://assets/of_wordmark_big.png"}}'
@onready var theme_json
@onready var pallete: Dictionary

signal change_to(game)


func set_button_colours(colour,time):
	var tween = get_tree().create_tween().set_parallel(true)
	tween.tween_property($TopPanel/AboutButton,"modulate",colour,time)
	tween.tween_property($TopPanel/AdvancedButton,"modulate",colour,time)
	tween.tween_property($TopPanel/HomeButton,"modulate",colour,time)
	tween.tween_property($TopPanel/DownloadTab,"modulate",colour,time)
	for x in $HBoxContainer.get_children():
		if x.get_class() == "TextureButton":
			tween.tween_property(x,"modulate",colour,time)


func _ready():
	var json_handler = JSON.new()
	var file = FileAccess.open("res://assets/themes.json", FileAccess.READ)
	theme_json = json_handler.parse_string(file.get_as_text())
	#theme = load("res://themes/pf2_theme.tres")
	apply_theme("adastral")
	add_side_icons()
	
	
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
	var tween = get_tree().create_tween().set_parallel(true)
	if pallete["id"] == "adastral":
		$TextureRect2.show()
	else:
		$TextureRect2.hide()
	var newbgtexture = load(pallete["bgpath"])
	var newstar = load(pallete["star"])
	$Panel2/Background.material.set_shader_parameter("weight",0)
	$Panel2/Background.material.set_shader_parameter("target_texture",newbgtexture)
	$TopPanel/AdastralLogo.material.set_shader_parameter("weight",0)
	$TopPanel/AdastralLogo.material.set_shader_parameter("target_texture",newstar)
	if not pallete.has("wordmark"):
		$Wordmark.material.set_shader_parameter("alpha",1)
		tween.tween_method(func(x): $Wordmark.material.set_shader_parameter("alpha",x),1.0,0.0,0.2)
	elif $Wordmark.is_visible(): #if it's visible, then there's an old and new texture so we need to tween weight.
		$Wordmark.material.set_shader_parameter("weight",0)
		$Wordmark.material.set_shader_parameter("target_texture",load(pallete["wordmark"]))
		tween.tween_method(func(x): $Wordmark.material.set_shader_parameter("weight",x),0.0,1.0,0.2)
	else: # if it's not visible, there's no texture next and we need to tween alpha.
		$Wordmark.texture = load(pallete["wordmark"])
		$Wordmark.show()
		$Wordmark.material.set_shader_parameter("alpha",0)
		tween.tween_method(func(x): $Wordmark.material.set_shader_parameter("alpha",x),0.0,1.0,0.2)
	tween.tween_method(func(x): $TopPanel/AdastralLogo.material.set_shader_parameter("weight",x),0.0,1.0,0.2)
	tween.tween_method(func(x): $Panel2/Background.material.set_shader_parameter("weight",x),0.0,1.0,0.2)
	tween.tween_method(func(x): $Install.add_theme_color_override("font_color",x),base_theme.get_color("font_color","ImportantButton"),Color(pallete["light"]),0.2)
	tween.tween_method(func(x): $Verify.add_theme_color_override("font_color",x),base_theme.get_color("font_color","Button"),Color(pallete["lightfg"]),0.2)
	tween.tween_method(func(x): $TopPanel/Adastral.add_theme_color_override("font_color",x),base_theme.get_color("font_color","TopLabels"),Color(pallete["light"]),0.2)
	tween.tween_method(func(x): $TopPanel/Adastral2.add_theme_color_override("font_color",x),base_theme.get_color("font_color","TopLabels"),Color(pallete["light"]),0.2)
	tween.tween_method(func(x): $InstalledVersion.add_theme_color_override("default_color",x),base_theme.get_color("default_color","PositiveRTL"),Color(pallete["light"]),0.2)
	tween.tween_method(func(x): $LatestVersion.add_theme_color_override("default_color",x),base_theme.get_color("default_color","PositiveRTL"),Color(pallete["light"]),0.2)
	tween.tween_property($BottomPanel.get_theme_stylebox("panel"),"bg_color", Color(pallete["main"]), 0.2)
	tween.tween_property($TopPanel.get_theme_stylebox("panel"),"bg_color", Color(pallete["dark"]), 0.2)
	tween.tween_property($SidePanel.get_theme_stylebox("panel"),"bg_color", Color(pallete["secondary"]), 0.2)
	tween.tween_property($Install.get_theme_stylebox("normal"),"bg_color", Color(pallete["accent"]), 0.2)
	tween.tween_property($Verify.get_theme_stylebox("normal"),"bg_color", Color(pallete["light"]), 0.2)
	tween.tween_property($ProgressBar.get_theme_stylebox("fill"),"bg_color", Color(pallete["accent"]), 0.2)
	tween.tween_property($ProgressBar.get_theme_stylebox("background"),"bg_color", Color(pallete["light"]), 0.2)
	#tween.tween_property($Install.get_theme_color("font_color"),"font_color", Color(pallete["lightfg"]), 0.2)
	set_button_colours(Color(pallete["light"]),0.2)
	await get_tree().create_timer(0.2).timeout
	if pallete.has("wordmark"):
		$Wordmark.texture = load(pallete["wordmark"]) # yes I know we load it twice but I'm not exactly the best at codeflow as you can see
	else:
		$Wordmark.material.set_shader_parameter("weight",0)
		$Wordmark.hide()
	$TopPanel/AdastralLogo.texture = newstar
	$Panel2/Background.texture = newbgtexture
	set_stylebox_colour(base_theme.get_stylebox("panel","AccentPanel"),Color(pallete["main"]))
	set_stylebox_colour(base_theme.get_stylebox("panel","TopPanel"),Color(pallete["dark"]))
	set_stylebox_colour(base_theme.get_stylebox("panel","SidePanel"),Color(pallete["secondary"]))
	set_stylebox_colour(base_theme.get_stylebox("normal","Button"),Color(pallete["light"]))
	set_stylebox_colour(base_theme.get_stylebox("normal","ImportantButton"),Color(pallete["accent"]))
	set_stylebox_colour(base_theme.get_stylebox("background","ProgressBar"),Color(pallete["light"]))
	set_stylebox_colour(base_theme.get_stylebox("fill","ProgressBar"),Color(pallete["accent"]))
	set_stylebox_colour(base_theme.get_stylebox("panel","TopPanel"),Color(pallete["dark"]))
	$Install.remove_theme_color_override("font_color")
	$Verify.remove_theme_color_override("font_color")
	$LatestVersion.remove_theme_color_override("font_color")
	$InstalledVersion.remove_theme_color_override("font_color")
	$TopPanel/Adastral.remove_theme_color_override("default_color")
	$TopPanel/Adastral2.remove_theme_color_override("default_color")
	base_theme.set_color("default_color","PositiveRTL",Color(pallete["light"]))
	base_theme.set_color("font_color","TopLabels",Color(pallete["light"]))
	base_theme.set_color("font_color","Button",Color(pallete["lightfg"]))
	base_theme.set_color("font_color","ImportantButton",Color(pallete["light"]))
	theme = base_theme
	
	

func add_new_sep():
	var new = VSeparator.new()
	new.size_flags_vertical = Control.SIZE_SHRINK_CENTER
	new.add_theme_stylebox_override("separator",StyleBoxEmpty.new())
	$HBoxContainer.add_child(new)

func add_side_icons():
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
	
	
	
