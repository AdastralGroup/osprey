extends Control
var pallete
var theme_json = {"adastral":
	{"id":"adastral","dark":"#00000000","light": "#f8f3ee",
		"main": "#0064ad",
		"accent": "#b76300",
		"lightfg": "#8d847b",
		"click":"#ffdd63",
		"click_t":"#000000",
		"bg": "res://assets/bg.png",
		"star": "res://assets/adastral.png"}}
var games: Dictionary
var s : binding
var current_screen : String # use an enum damnit!!!!
var current_game = ""
signal change_to(game)
signal update_game(game)
signal verify_game(game)
var oops_s = preload("res://oops.tscn")
var panic_s = preload("res://panic.tscn")

func panic(err_string):
	var z = panic_s.instantiate()
	z.find_child("Label2").set("text",err_string)
	get_tree().get_root().add_child(z)
	await get_tree().create_timer(1).timeout
	get_tree().paused = true
	
func oops(err_string):
	var z = oops_s.instantiate()
	z.find_child("Label2").set("text",err_string)
	get_tree().get_root().add_child(z)
	await get_tree().create_timer(1).timeout
	get_tree().paused = true

func load_image(path):
	if path == "":
		panic("no image path specified?")
		return
	if "res://" in path:
		return load(path)
	else:
		return ImageTexture.create_from_image(Image.load_from_file(path))

func _ready():
	pass
	#theme = load("res://themes/pf2_theme.tres")
	
func _on_Button4_pressed():
	pass

func _on_progress_update(game,progress):
	call_deferred("t_progress_update",game,progress)

func _on_HomeButton_pressed():
	apply_theme("adastral")

func _on_game_verified(status,game):
	print("[Belmont/GameVerified] %s verified" % [game])
	call_deferred("t_game_verified",game)
	
func _on_game_updated(status,game):
	call_deferred("t_game_updated",game)
	
func _on_error(error_detail):
	print(error_detail)
	
func _on_install_pressed():
	games[current_game]["status"] = "installing"
	s.update_game(current_game)
	$Install.disabled = true
	$Install.text = "Installing..."
	$Verify.disabled = true
	var tween = get_tree().create_tween()
	$ProgressBar.show()
	tween.tween_property($ProgressBar,"modulate",Color.WHITE,0.2)

func _on_verify_pressed():
	games[current_game]["status"] = "verifying"
	print("[Belmont/VerifyPressed] Verifying %s" % [current_game])
	s.verify_game(current_game)
	$Install.disabled = true
	$Verify.text = "Verifying..."
	$Verify.disabled = true
	var tween = get_tree().create_tween()
	$ProgressBar.show()
	tween.tween_property($ProgressBar,"modulate",Color.WHITE,0.2)
	
	
func set_button_colours(colour,time):
	var tween = get_tree().create_tween().set_parallel(true)
	tween.tween_property($TopPanel/AdvancedButton,"modulate",colour,time)
	tween.tween_property($TopPanel/HomeButton,"modulate",colour,time)
	for x in $HBoxContainer.get_children():
		if x.get_class() == "TextureButton":
			tween.tween_property(x,"modulate",colour,time)


func change_game(game):
	if "progress" in games[game]:
		if games[game]["progress"] != 0:
			$ProgressBar.show()
			$ProgressBar.value = games[game]["progress"]
		else:
			$ProgressBar.hide()
	apply_theme(game)


func ready_after_sutton():
	apply_theme("adastral")
	s.connect("game_verified",_on_game_verified)
	s.connect("game_updated",_on_game_updated)
	s.connect("progress_update",_on_progress_update)
	s.connect("error",_on_error)
	var z = s.get_server_games()
	for x in z:
		theme_json[x] = s.get_game_assets(x)
	add_side_icons()

func add_new_sep():
	var new = VSeparator.new()
	new.size_flags_vertical = Control.SIZE_SHRINK_CENTER
	new.add_theme_stylebox_override("separator",StyleBoxEmpty.new())
	$HBoxContainer.add_child(new)

func add_side_icons():
	for x in theme_json.values():
		if x["id"] != "adastral":
			games[x["id"]] = {}
			if $HBoxContainer.get_child_count() != 0:
				add_new_sep()
			var new = TextureButton.new()
			new.ignore_texture_size = true
			new.stretch_mode = TextureButton.STRETCH_SCALE
			new.custom_minimum_size= Vector2(42,42)
			new.size_flags_horizontal = Control.SIZE_SHRINK_CENTER 
			new.size_flags_vertical = Control.SIZE_SHRINK_CENTER
			new.texture_normal = load_image(x["icon"])
			$HBoxContainer.add_child(new)
			new.show()
			new.pressed.connect(func(): change_game(x["id"]))
	


func t_progress_update(game,progress):
	games[game]["progress"] = progress*100

func t_game_verified(game):
	change_game(game)
	games[game]["progress"] = 0
	if current_game == game:
		var tween = get_tree().create_tween()
		tween.tween_property($ProgressBar,"modulate",Color.TRANSPARENT,0.2)
		await get_tree().create_timer(0.2).timeout
		$ProgressBar.hide()

func t_game_updated(game):
	change_game(game)
	$InstalledVersion.text = "[left]Installed Version: [b]%s[/b]" % s.get_installed_version(game)
	games[game]["progress"] = 0
	if current_game == game:
		var tween = get_tree().create_tween()
		tween.tween_property($ProgressBar,"modulate",Color.TRANSPARENT,0.2)
		await get_tree().create_timer(0.2).timeout
		$ProgressBar.hide()



func disabled(color):
	var temp = color
	temp.a = 0.3
	return temp

func set_stylebox_colour(stylebox,colour):
	stylebox.bg_color = colour



func set_buttons(game_name):
	if s.get_installed_version(game_name) == "":
		if game_name in games.keys():
			if "status" in games[game_name].keys():
				if games[game_name]["status"] == "installing":
					$Install.text = "Installing.."
			else:
				$Install.text = "Install"
		$InstalledVersion.text = "[left]Not Installed!"
		$Verify.disabled = true
		$Install.disabled = false
		
	else:
		$InstalledVersion.text = "[left]Installed Version: [b]%s[/b]" % s.get_installed_version(game_name)
		if s.get_installed_version(game_name) == s.get_latest_version(game_name):
			$Install.disabled = true
			$Install.text = "Installed"
			$Verify.disabled = false
			$Verify.text = "Verify"
		elif int(s.get_installed_version(game_name)) < int(s.get_latest_version(game_name)):
			$Verify.text = "Verify"
			$Install.disabled = false
			$Verify.disabled = false
			$Install.text = "Update"
		if game_name in games.keys(): # this is insanely cooked
			if "status" in games[game_name].keys():
				if games[game_name]["status"] == "installing":
					$Install.text = "Installing.."
			else:
				$Install.text = "Install"
		


func apply_theme(theme_name):
	pallete = theme_json[theme_name]
	var base_theme = load("res://themes/base.tres") ## we shouldn't be doing this
	if not pallete.has("secondary"):
		pallete["secondary"] = pallete["main"]
	var tween = get_tree().create_tween().set_parallel(true)
	if pallete["id"] == "adastral":
		#$TextureRect2.show() 
		$LatestVersion.hide()
		$InstalledVersion.hide()
		$Install.hide()
		$Verify.hide()
		$Panel2/Background.hide()
		$AboutInfo/BigLabel.visible_characters = -1
		$AboutInfo.show()
		if get_node_or_null("TextureRect"):
			$TextureRect.show()
		$BottomPanel.hide()
	else:
		if get_node_or_null("TextureRect"):
			$TextureRect.hide()
		$AboutInfo.hide()
		$BottomPanel.show() # these all need to be tweens
		$Install.show()
		$Verify.show()
		$LatestVersion.show()
		$InstalledVersion.show()
		$TextureRect2.hide()
		$Panel2/Background.show()
		#$Panel2.show()
	$LatestVersion.text = "[left]Latest Version: [b]%s[/b]" % s.get_latest_version(theme_name)
	set_buttons(theme_name)
	var newbgtexture = load_image(pallete["bg"])
	var newstar = load_image(pallete["star"])
	$Panel2/Background.material.set_shader_parameter("weight",0)
	$Panel2/Background.material.set_shader_parameter("target_texture",newbgtexture)
	$TopPanel/AdastralLogo.material.set_shader_parameter("weight",0)
	$TopPanel/AdastralLogo.material.set_shader_parameter("target_texture",newstar)
	
	if not pallete.has("wordmark"):
		$Wordmark.material.set_shader_parameter("alpha",1)
		tween.tween_method(func(x): $Wordmark.material.set_shader_parameter("alpha",x),1.0,0.0,0.2)
	elif $Wordmark.is_visible(): #if it's visible, then there's an old and new texture so we need to tween weight.
		$Wordmark.material.set_shader_parameter("weight",0)
		$Wordmark.material.set_shader_parameter("target_texture",load_image(pallete["wordmark"]))
		tween.tween_method(func(x): $Wordmark.material.set_shader_parameter("weight",x),0.0,1.0,0.2)
	else: # if it's not visible, there's no texture next and we need to tween alpha.
		$Wordmark.texture = load_image(pallete["wordmark"])
		$Wordmark.show()
		$Wordmark.material.set_shader_parameter("alpha",0)
		tween.tween_method(func(x): $Wordmark.material.set_shader_parameter("alpha",x),0.0,1.0,0.2)
	tween.tween_method(func(x): $TopPanel/AdastralLogo.material.set_shader_parameter("weight",x),0.0,1.0,0.2)
	tween.tween_method(func(x): $Panel2/Background.material.set_shader_parameter("weight",x),0.0,1.0,0.2)
	tween.tween_method(func(x): $Install.add_theme_color_override("font_color",x),base_theme.get_color("font_color","ImportantButton"),Color(pallete["light"]),0.2)
	tween.tween_method(func(x): $Verify.add_theme_color_override("font_color",x),base_theme.get_color("font_color","Button"),Color(pallete["lightfg"]),0.2)
	tween.tween_method(func(x): $Verify.add_theme_color_override("font_hover_color",x),base_theme.get_color("font_hover_color","Button"),Color(pallete["lightfg"]),0.2)
	tween.tween_method(func(x): $TopPanel/Adastral.add_theme_color_override("font_color",x),base_theme.get_color("font_color","TopLabels"),Color(pallete["light"]),0.2)
	tween.tween_method(func(x): $TopPanel/Adastral2.add_theme_color_override("font_color",x),base_theme.get_color("font_color","TopLabels"),Color(pallete["light"]),0.2)
	tween.tween_method(func(x): $InstalledVersion.add_theme_color_override("default_color",x),base_theme.get_color("default_color","PositiveRTL"),Color(pallete["light"]),0.2)
	tween.tween_method(func(x): $LatestVersion.add_theme_color_override("default_color",x),base_theme.get_color("default_color","PositiveRTL"),Color(pallete["light"]),0.2)
	tween.tween_property($BottomPanel.get_theme_stylebox("panel"),"bg_color", Color(pallete["main"]), 0.2)
	tween.tween_property($TopPanel.get_theme_stylebox("panel"),"bg_color", Color(pallete["dark"]), 0.2)
	tween.tween_property($SidePanel.get_theme_stylebox("panel"),"bg_color", Color(pallete["secondary"]), 0.2)
	tween.tween_property($Install.get_theme_stylebox("normal"),"bg_color", Color(pallete["accent"]), 0.2)
	tween.tween_property($Install.get_theme_stylebox("hover"),"bg_color", Color(pallete["accent"]), 0.2)
	tween.tween_property($Verify.get_theme_stylebox("normal"),"bg_color", Color(pallete["light"]), 0.2)
	tween.tween_property($Verify.get_theme_stylebox("hover"),"bg_color", Color(pallete["light"]), 0.2)
	##tween.tween_property($Install.get_theme_stylebox("disabled"),"bg_color",Color(pallete["dark"],0.2) tween disabled when we need to
	tween.tween_property($Verify.get_theme_stylebox("pressed"),"bg_color", Color(pallete["click"]), 0.2)
	tween.tween_property($Verify.get_theme_stylebox("pressed"),"bg_color", Color(pallete["click"]), 0.2)
	tween.tween_property($ProgressBar.get_theme_stylebox("fill"),"bg_color", Color(pallete["accent"]), 0.2)
	tween.tween_property($ProgressBar.get_theme_stylebox("background"),"bg_color", Color(pallete["light"]), 0.2)
	set_button_colours(Color(pallete["light"]),0.2)
	await get_tree().create_timer(0.2).timeout
	if pallete.has("wordmark"):
		$Wordmark.texture = load_image(pallete["wordmark"]) # yes I know we load it twice but I'm not exactly the best at codeflow as you can see
	else:
		$Wordmark.material.set_shader_parameter("weight",0)
		$Wordmark.hide()
	$TopPanel/AdastralLogo.texture = newstar
	$Panel2/Background.texture = newbgtexture
	set_stylebox_colour(base_theme.get_stylebox("panel","AccentPanel"),Color(pallete["main"]))
	set_stylebox_colour(base_theme.get_stylebox("panel","TopPanel"),Color(pallete["dark"]))
	set_stylebox_colour(base_theme.get_stylebox("panel","SidePanel"),Color(pallete["secondary"]))
	set_stylebox_colour(base_theme.get_stylebox("normal","Button"),Color(pallete["light"]))
	set_stylebox_colour(base_theme.get_stylebox("disabled","Button"),disabled(Color(pallete["light"])))
	set_stylebox_colour(base_theme.get_stylebox("pressed","Button"),Color(pallete["click"]))
	set_stylebox_colour(base_theme.get_stylebox("normal","ImportantButton"),Color(pallete["accent"]))
	set_stylebox_colour(base_theme.get_stylebox("disabled","ImportantButton"),disabled(Color(pallete["accent"])))
	set_stylebox_colour(base_theme.get_stylebox("pressed","ImportantButton"),Color(pallete["click_t"]))
	set_stylebox_colour(base_theme.get_stylebox("background","ProgressBar"),Color(pallete["light"]))
	set_stylebox_colour(base_theme.get_stylebox("fill","ProgressBar"),Color(pallete["accent"]))
	set_stylebox_colour(base_theme.get_stylebox("panel","TopPanel"),Color(pallete["dark"]))
	$Install.remove_theme_color_override("font_color")
	$Verify.remove_theme_color_override("font_color")
	$Verify.remove_theme_color_override("font_hover_color")
	$Verify.remove_theme_color_override("font_focus_color")
	$Verify.remove_theme_color_override("font_pressed_color")
	$LatestVersion.remove_theme_color_override("font_color")
	$InstalledVersion.remove_theme_color_override("font_color")
	$TopPanel/Adastral.remove_theme_color_override("default_color")
	$TopPanel/Adastral2.remove_theme_color_override("default_color")
	base_theme.set_color("default_color","PositiveRTL",Color(pallete["light"]))
	base_theme.set_color("font_color","TopLabels",Color(pallete["light"]))
	base_theme.set_color("font_color","Button",Color(pallete["lightfg"]))
	base_theme.set_color("font_hover_color","Button",Color(pallete["lightfg"]))
	base_theme.set_color("font_pressed_color","Button",Color(pallete["click_t"]))
	base_theme.set_color("font_disabled_color","Button",Color(pallete["lightfg"]))
	base_theme.set_color("font_color","ImportantButton",Color(pallete["light"]))
	theme = base_theme
	current_game = theme_name
	
func _process(delta):
	if games != {} and current_game != "" and current_game != "adastral":
		if "progress" in games[current_game].keys():
			if games[current_game]["progress"] != 0:
				$ProgressBar.value = games[current_game]["progress"]
