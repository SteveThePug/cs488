-- A simple scene with five spheres

mat1 = gr.material({ 0.0, 1.0, 0.7 }, { 0.5, 0.7, 0.5 }, 25)
mat2 = gr.material({ 0.5, 0.5, 0.5 }, { 0.5, 0.7, 0.5 }, 25)
mat3 = gr.material({ 1.0, 0.6, 0.1 }, { 0.5, 0.7, 0.5 }, 25)

scene_root = gr.node("root")

s1 = gr.nh_sphere("s1", { 0, 0, 0 }, 1)
scene_root:add_child(s1)
s1:set_material(mat1)

s2 = gr.sphere("s2", { 0, -1, 0 }, 0.5)
scene_root:add_child(s2)
s2:set_material(mat2)
s2:rotate("Z", 90)
s2:translate(-2, 0, 0)

s3 = gr.sphere("s3")
s2:add_child(s3)
s3:translate(0, -2, 0)

s4 = gr.cube("s4")
scene_root:add_child(s4)
s4:set_material(mat3)
s4:rotate("X", 45)
s4:rotate("Y", 46)

s5 = gr.nh_box("s5", { 0, 0, 0 }, 0.5)
scene_root:add_child(s5)
s5:set_material(mat1)
s5:translate(2, 2, 0)

white_light = gr.light({ 10.0, 10.0, 10.0 }, { 0.9, 0.9, 0.2 }, { 1, 0, 0 })
magenta_light = gr.light({ -10.0, -10.0, -10.0 }, { 0.7, 0.0, 0.7 }, { 1, 0, 0 })

gr.render(
	scene_root,
	"simple.png",
	1080,
	1440,
	{ 0, 0, 10 },
	{ 0, 0, -1 },
	{ 0, 1, 0 },
	50,
	{ 0.3, 0.3, 0.3 },
	{ white_light, magenta_light }
)
