-- Materials
material_skin = gr.material({ 0.95, 0.85, 0.75 }, { 0.1, 0.8, 0.8 }, 0.8)
material_tears = gr.material({ 0, 0, 1 }, { 0.1, 0.8, 0.8 }, 0.8)
material_pupils = gr.material({ 0, 0, 0 }, { 0.1, 0.8, 0.8 }, 0.8)
material_mouth = gr.material({ 0.3, 0, 0.1 }, { 0.1, 0.8, 0.8 }, 0.8)
material_eyes = gr.material({ 1, 1, 1 }, { 0.1, 0.8, 0.8 }, 0.8)

-- Degrees of freedom
dof_legX = { -45, 0, 45 }
dof_legY = { 0, 0, 0 }
dof_armX = { -90, 0, 90 }
dof_armY = { 0, 0, 0 }
dof_headX = { -45, 0, 45 }
dof_headY = { -45, 0, 45 }
dof_pupilX = { -45, 0, 45 }
dof_pupilY = { -45, 0, 45 }

-- Create the top level root node named 'root'
rootNode = gr.node("root")

-- Create the body GeometryNode
body = gr.mesh("Body", "body")
body:set_material(material_skin)
rootNode:add_child(body)

-- Left leg
l_legJoint = gr.joint("l_legJoint", dof_legX, dof_legY)
l_legJoint:translate(-0.2, -0.2, 0)
body:add_child(l_legJoint)

l_leg = gr.mesh("Joint", "l_leg")
l_leg:set_material(material_skin)
l_leg:translate(0, -0.4, 0)
l_legJoint:add_child(l_leg)

l_footJoint = gr.joint("l_footJoint", dof_armX, dof_armY)
l_footJoint:translate(0, -0.05, 0)
l_leg:add_child(l_footJoint)

l_foot = gr.mesh("Joint", "l_foot")
l_foot:set_material(material_skin)
l_foot:translate(0, -0.4, 0)
l_footJoint:add_child(l_foot)

-- Right leg
r_legJoint = gr.joint("r_legJoint", dof_legX, dof_legY)
r_legJoint:translate(0.2, -0.2, 0)
body:add_child(r_legJoint)

r_leg = gr.mesh("Joint", "r_leg")
r_leg:set_material(material_skin)
r_leg:translate(0, -0.4, 0)
r_legJoint:add_child(r_leg)

r_footJoint = gr.joint("r_footJoint", dof_armX, dof_armY)
r_footJoint:translate(0, -0.05, 0)
r_leg:add_child(r_footJoint)

r_foot = gr.mesh("Joint", "r_foot")
r_foot:set_material(material_skin)
r_foot:translate(0, -0.4, 0)
r_footJoint:add_child(r_foot)

-- Left arm
l_armJoint = gr.joint("l_armJoint", dof_armX, dof_armY)
l_armJoint:translate(-0.5, 0.3, 0)
body:add_child(l_armJoint)

l_arm = gr.mesh("Joint", "l_arm")
l_arm:set_material(material_skin)
l_arm:translate(0, -0.4, 0)
l_armJoint:add_child(l_arm)

l_handJoint = gr.joint("l_handJoint", dof_armX, dof_armY)
l_handJoint:translate(0, -0.05, 0)
l_arm:add_child(l_handJoint)

l_hand = gr.mesh("Joint", "l_hand")
l_hand:set_material(material_skin)
l_hand:translate(0, -0.4, 0)
l_handJoint:add_child(l_hand)

-- Right arm
r_armJoint = gr.joint("r_armJoint", dof_armX, dof_armY)
r_armJoint:translate(0.5, 0.3, 0)
body:add_child(r_armJoint)

r_arm = gr.mesh("Joint", "r_arm")
r_arm:set_material(material_skin)
r_arm:translate(0, -0.4, 0)
r_armJoint:add_child(r_arm)

r_handJoint = gr.joint("r_handJoint", dof_armX, dof_armY)
r_handJoint:translate(0, -0.05, 0)
r_arm:add_child(r_handJoint)

r_hand = gr.mesh("Joint", "r_hand")
r_hand:set_material(material_skin)
r_hand:translate(0, -0.4, 0)
r_handJoint:add_child(r_hand)

-- Head
headJoint = gr.joint("headJoint", dof_headX, dof_headY)
headJoint:translate(0, 0.2, 0)
body:add_child(headJoint)

head = gr.mesh("Head", "head")
head:set_material(material_skin)
head:translate(0, 0.5, 0)
headJoint:add_child(head)

-- Mouth
mouth = gr.mesh("Mouth", "mouth")
mouth:translate(0, -0.2, 0.35)
mouth:set_material(material_mouth)
head:add_child(mouth)

-- Left eye
l_eye = gr.mesh("Eye", "l_eye")
l_eye:set_material(material_eyes)
l_eye:translate(-0.3, 0.1, 0.3)
head:add_child(l_eye)

-- Left pupil
l_pupilJoint = gr.joint("l_pupilJoint", dof_pupilX, dof_pupilY)
l_pupilJoint:translate(-0.3, 0.1, 0.3)
head:add_child(l_pupilJoint)

l_pupil = gr.mesh("Pupil", "l_pupil")
l_pupil:set_material(material_pupils)
l_pupil:translate(0, 0, 0.1)
l_pupilJoint:add_child(l_pupil)

-- Left tear
l_tear = gr.mesh("Joint", "l_tear")
l_tear:set_material(material_tears)
l_tear:translate(-0.3, -0.3, 0.3)
head:add_child(l_tear)

-- Right eye
r_eye = gr.mesh("Eye", "r_eye")
r_eye:set_material(material_eyes)
r_eye:translate(0.3, 0.1, 0.3)
head:add_child(r_eye)

-- Right pupil
r_pupilJoint = gr.joint("r_pupilJoint", dof_pupilX, dof_pupilY)
r_pupilJoint:translate(0.3, 0.1, 0.3)
head:add_child(r_pupilJoint)

r_pupil = gr.mesh("Pupil", "r_pupil")
r_pupil:set_material(material_pupils)
r_pupil:translate(0, 0, 0.1)
r_pupilJoint:add_child(r_pupil)

-- Right tear
r_tear = gr.mesh("Joint", "r_tear")
r_tear:set_material(material_tears)
r_tear:translate(0.3, -0.35, 0.3)
head:add_child(r_tear)

return rootNode
