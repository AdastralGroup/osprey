extends Control


func _on_button_pressed() -> void:
	var t = get_tree().create_tween()
	t.tween_property($Camera2D,"position",Vector2(770,0),1).set_trans(Tween.TRANS_QUINT).set_ease(Tween.EASE_OUT)

func _on_default_button_pressed() -> void:
	pass # Replace with function body.


func _on_custom_button_pressed() -> void:
	$FileDialog.popup()



func _on_file_dialog_dir_selected(dir: String) -> void:
	print($FileDialog.current_path)
