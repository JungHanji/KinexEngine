#include <Kinex/app.hpp>

int main(){
    auto strres = input("Enter resolution in format 'X Y', or enter 'auto' for auto detection: ");
    int x, y;
    if(strres == "auto"){
        x = GetSystemMetrics(SM_CXSCREEN);
        y = GetSystemMetrics(SM_CYSCREEN);
    } else {
        auto res = split(strres);
        if(res.size() != 2) return 0;
        x = stoi(res[0]);
        y = stoi(res[1]);
    }
    knx::Core core(
        {x, y},
        {0.8f, 0.8f, 0.8f, 1.0f}, 
        "Object test", 
        {0.05f, 0.7f},
        false
    );
    core.setupCamera({{0, 0, 0}}, 50, 0.25f, 0.1f, 5000.f);
    // core.setupPostprocessingFromFile("res/shaders/postrpoc/screen");
    core.setupSkyBox("res/textures/skyboxes/default");
    core.setupFog({0.8f, 0.8f, 0.8f}, 0.02f);

    bool showDepthQuad = false;
    core.getInputSystem().registrateAction("cursorModes", {knx::MOUSE_EVENT_, [&](const knx::irl::Event &event){ if((knx::MOUSE_EVENTS)event.getCode() == knx::MOUSE_PRESSED_){if(event.mouseButton == 0){core.getInputSystem().hideCursor(); core.getInputSystem().fixCursor(core.getWindow().getResolution() / 2.f); }}}});
    core.getInputSystem().registrateAction("esc", {knx::KEYBOARD_EVENT_, [&](const knx::irl::Event &event){if((knx::KEYBOARD_EVENTS)event.getCode() == knx::KEY_PRESSED_){ if(event.key == GLFW_KEY_ESCAPE){ core.getInputSystem().showCursor(); core.getInputSystem().freeCursor();}}}});
    core.getInputSystem().registrateAction("getCameraPos", {knx::KEYBOARD_EVENT_, [&](const knx::irl::Event &event){if((knx::KEYBOARD_EVENTS)event.getCode() == knx::KEY_PRESSED_){ if(event.key == GLFW_KEY_F1){ cout<<"[INFO] Camera position: "<<core.getCamera().getTransform().getPosition().str()<<endl;}}}});
    core.getInputSystem().registrateAction("showDepth", {knx::KEYBOARD_EVENT_, [&](const knx::irl::Event &event){if((knx::KEYBOARD_EVENTS)event.getCode() == knx::KEY_PRESSED_){ if(event.key == GLFW_KEY_F2){showDepthQuad = !showDepthQuad; core.getCamera().getTransform().setPosition({0.0f, 10.0f, 0.0f});}}}});
    core.setCAController(knx::CameraController( core.getCameraPointer(), core.getInputSystemPointer(), core.getTimePointer()));
    
    core.getLightScene().addPointLightSource("sun", knx::PointLightSource( knx::irl::PointLight( {0.2f}, {0.5f}, {1.0f}, 0.14f, 0.07f, {1.f} ), knx::irl::Transform(vec3f{0.f, 2.f, 0.f}) ) );
    core.getLightScene().addDirectionLightSource("sun2", knx::DirectLightSource( knx::irl::DirectionLight( {0.0f}, {1.f}, {0.3f}, {0.5} ), knx::irl::Transform({0, 37, -7}, {0.f, -.5f, 0.f}) ) );
    core.getLightScene().addSpotLightSource("sun3", knx::SpotLightSource( knx::irl::SpotLight( {0.1f}, {.8f}, {1.f}, cos(rad(12.5)), cos(rad(17.5)), 0.09, 0.032, {0, 1, 0} ), knx::irl::Transform({1}, {0.f, 1.f, 0.f}) ) );
    
    auto shader = knx::irl::Shader( "res/shaders/shadows/grobj" );
    // auto shader = knx::irl::Shader( "res/shaders/st_shaders/StandartShader" );
    core.addShader("objectShader", &shader);

    auto mesh = knx::irl::Mesh(knx::irl::meshes::cubemesh_txs_nrms, true, true);
    mesh.scaleTexturePoses({5.f, 5.f});
    mesh.setupBuffers(*core.getShaderPointer("objectShader"));

    core.addMaterial("cubeMaterial", knx::irl::Material(32.0f, {0.f, 0.31f, 0.7f}, {0.5f}));
    core.addTexture("mosaic", knx::irl::Texture("res/textures/single_textures/mosaic.jpg", true));
    knx::Object cube(
        "cube-1",
        knx::irl::Transform(
            {0, 0, 0}, // Position
            {0, 0, 0}  // Rotation
        ), {{"currtexture", core.getTexture("mosaic")}},
        core.getCameraPointer(),
        core.getShaderPointer("objectShader"),
        core.getMaterialPointer("cubeMaterial")
    );
    cube.meshPtr = &mesh;
    core.addObject("cube-1", &cube);

    knx::Object cube3(
        "cube-3",
        knx::irl::Transform(
            {10, -10, 10}, // Position
            {50, 0.5, 100},  // Rotation
            {15, 15, 15}
        ), {{"currtexture", core.getTexture("mosaic")}},
        core.getCameraPointer(),
        core.getShaderPointer("objectShader"),
        core.getMaterialPointer("cubeMaterial")
    );
    cube3.meshPtr = &mesh;
    core.addObject("cube-3", &cube3);

    knx::Object floor(
        "floor",
        knx::irl::Transform(
            {0, -5, 0}, // Position
            {0, 0, 0}, // Rotation
            {50, 1, 50}
        ), {{"currtexture", core.getTexture("mosaic")}},
        core.getCameraPointer(),
        core.getShaderPointer("objectShader"),
        core.getMaterialPointer("cubeMaterial")
    );

    floor.meshPtr = &mesh;
    core.addObject("floor", &floor);

    while(core.isRunning()){ core.update(
            [&](){ cube.getTransform().rotate(vec3f{0, 0.01, 0}); cube3.getTransform().rotate(vec3f{0.001, 0.001, 0.001});},
            [&](){
                core.getLightScene().getSpotLightSource("sun3").getTransform().setPosition(core.getCamera().getTransform().getPosition());
                core.getLightScene().getSpotLightSource("sun3").getTransform().setRotation(core.getCamera().getTransform().getRotation());

                // core.getLightScene().getDirectionLightSource("sun2").getTransform().setRotation(core.getCamera().getTransform().getRotation());
                // core.getLightScene().getDirectionLightSource("sun2").getTransform().setPosition(core.getCamera().getTransform().getPosition());
            },
            [&](){}, [&](){}, showDepthQuad
        );
    }
}