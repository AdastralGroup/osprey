extends Control


func do():
	$Panel.position.y = -350
	$RichTextLabel.hide()
	$TextureRect.hide()
	$Label.hide()
	$Label2.hide()
	var tween = get_tree().create_tween()
	tween.tween_property($Panel,"position",Vector2(0,0),1)
	await get_tree().create_timer(1).timeout
	$RichTextLabel.show()
	$TextureRect.show()
	$Label.show()
	$Label2.show()
	
	


func _ready():
	do()
