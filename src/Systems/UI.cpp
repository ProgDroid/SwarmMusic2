#include <Systems/UI.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

#include <iomanip>
#include <sstream>

namespace Systems {

UI::UI() {}

UI::~UI() {
	nk_glfw3_shutdown(&glfw);
}

void UI::init(GLFWwindow* window) {
    context = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);

	nk_glfw3_font_stash_begin(&glfw, &atlas);
	nk_glfw3_font_stash_end(&glfw);
}

void UI::update(Managers::Properties* propertiesManager, float fps) {
	nk_glfw3_new_frame(&glfw); 

	// draw non-transparent background stuff
	context->style.window.fixed_background.data.color.a = 255;

	drawSwarmProperties(
        propertiesManager->getRadiusRepulsion(),
        propertiesManager->getRadiusOrientation(),
        propertiesManager->getRadiusAttraction(),
        propertiesManager->getBlindAngle(),
        propertiesManager->getSpeed(),
        propertiesManager->getMaxForce()
    );

	drawMusicalProperties();

	// draw transparent background stuff
	context->style.window.fixed_background.data.color.a = 0;

	drawFpsDisplay(fps);

	nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}

// ? MIGHT NEED WORK
void UI::drawFpsDisplay(float fps) {
	if (
		nk_begin(
			context,
            "fps",
            nk_rect((glfw.display_width / 2) - 15, 0, 30, 25),
            NK_WINDOW_NO_INPUT | NK_WINDOW_NO_SCROLLBAR
		)
	) {
        std::stringstream fpsStream;
        fpsStream << std::fixed << std::setprecision(0) << fps;

        nk_layout_row_dynamic(context, 15, 1);
        nk_label(context, fpsStream.str().c_str(), NK_TEXT_RIGHT);
    }
    nk_end(context);
}

// ! NEEDS WORK
void UI::drawMusicalProperties() {
	if (
		nk_begin(
			context,
			"Musical Properties",
			nk_rect(glfw.display_width - 285, 0, 285, 245),
			NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_NO_SCROLLBAR
		)
    ) {
		// ! move data to json
        // style picker
        std::vector<std::pair<std::string, unsigned int>> stylePicker {
            {"DOOM", 0}, // DOOM
            {"Jazz", 1}, // JAZZ
            {"Pop", 2}, // POP
            {"Metal", 3}, // METAL
            {"Punk", 4} // PUNK
        };

        picker("Style:", stylePicker, &style);

        // swarm mode picker (won't use picker as this uses swarm.setSwarmMode)
        // it's also not terribly long
        // nk_layout_row_dynamic(context, 15, 1);
        // nk_label(context, "Swarm Mode:", NK_TEXT_LEFT);
        // nk_layout_row_dynamic(context, 15, 2);
        // if (nk_option_label(context, "Random",  swarm.getSwarmMode() == RANDOM))  swarm.setSwarmMode(RANDOM);
        // if (nk_option_label(context, "Average", swarm.getSwarmMode() == AVERAGE)) swarm.setSwarmMode(AVERAGE);

		std::vector<std::pair<std::string, unsigned int>> swarmModePicker {
            {"Random", 0}, // RANDOM
            {"Average", 1}, // AVERAGE
        };

		// ? use picker
		picker("Swarm Mode:", swarmModePicker, &swarmMode);

        // pitch selector
        pitchSelector();

        // scale picker
        std::vector<std::pair<std::string, unsigned int>> scalePicker {
            {"Major", 0}, // MAJOR
            {"minor", 1}, // MINOR
            {"Pentatonic", 2}, // PENTATONIC
            {"Blues", 3} // BLUES
        };

        picker("Scale:", scalePicker, &scale);

        // reset style for following buttons
        resetButtonStyle();

        // add attractor
        nk_layout_row_dynamic(context, 20, 2);
        if (nk_button_label(context, "Add Attractor")) {
            // swarm.addAttractor((int) GUIpitch + C_MIDI_PITCH, VBO, EBO, VAO, -1);
        }

        // add scale
        if (nk_button_label(context, "Add Scale")) {
            // makeScale((int) GUIpitch + C_MIDI_PITCH, VBO, EBO, VAO);
        }

        // mute (will override button style if muted)
        muteButton();
    }
    nk_end(context);
}

void UI::drawSwarmProperties(
    int* repulsionRadius,
    int* orientationRadius,
    int* attractionRadius,
    int* blindAngle,
    float* speed,
    float* maxForce
) {
	if (
		nk_begin(
			context,
            "Swarm Properties",
            nk_rect(0, 0, 285, 155),
            NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_NO_SCROLLBAR
		)
	) {
        context->style.slider.cursor_size = nk_vec2(14, 14);

        islider("Repulsion:", 10, 40, repulsionRadius, 1);

        islider("Orientation:", 41, 70, orientationRadius, 1);

        islider("Attraction:", 71, 150, attractionRadius, 1);

        islider("Blind Angle:", 0, 45, blindAngle, 1);

        fslider("Speed:", 10, 63, speed, 0.1);

        fslider("Force:", 10, 50, maxForce, 0.1);
    }
    nk_end(context);
}

// ! NEEDS WORK
void UI::muteButton() {
	nk_layout_row_dynamic(context, 25, 1);

    // style button if muted
    context->style.button.normal      = mute ? nk_style_item_color(nk_rgb(255, 90, 95)) : nk_style_item_color(nk_rgb(45, 45, 45)); // 193, 131, 159?
    context->style.button.text_normal = mute ? nk_rgb(225, 225, 225) : nk_rgb(175, 175, 175);
    context->style.button.hover       = mute ? nk_style_item_color(nk_rgb(245, 80, 85)) : nk_style_item_color(nk_rgb(40, 40, 40));
    context->style.button.text_hover  = mute ? nk_rgb(225, 225, 225) : nk_rgb(175, 175, 175);

    if (nk_button_label(context, mute ? "Unmute" : "Mute")) {
        mute = !mute;
    }
}

// ! NEEDS WORK
void UI::picker(std::string label, std::vector<std::pair<std::string, unsigned int>> options, unsigned int *variable) {
	nk_layout_row_dynamic(context, 15, 1);
    nk_label(context, label.c_str(), NK_TEXT_LEFT);
    nk_layout_row_dynamic(context, 15, options.size());

    for (int i = 0, size = options.size(); i < size; ++i) {
        if (nk_option_label(context, options.at(i).first.c_str(), *variable == options.at(i).second)) {
			*variable = options.at(i).second;
		}
    }
}

// ! NEEDS WORK
void UI::pitchSelector() {
	std::vector<const char *> pitches = {
        "C4", "C#4", "D4", "D#4",
        "E4", "F4", "F#4", "G4",
        "G#4", "A4", "A#4", "B4",
        "C5", "C#5", "D5", "D#5",
        "E5", "F5", "F#5", "G5",
        "G#5", "A5", "A#5", "B5",
        "C6"
    };

    nk_layout_row_dynamic(context, 15, 1);
    nk_label(context, "Selected Pitch:", NK_TEXT_LEFT);
    if (nk_combo_begin_label(context, pitches[GUIpitch], nk_vec2(nk_widget_width(context), 197))) {
        nk_layout_row_dynamic(context, 15, 1);
        for (unsigned int i = 0; i < pitches.size(); ++i) {
            // highlight pitch in use
            context->style.contextual_button.normal      = i == GUIpitch ? nk_style_item_color(nk_rgb(255, 90, 95)) : nk_style_item_color(nk_rgb(45, 45, 45));
            context->style.contextual_button.text_normal = i == GUIpitch ? nk_rgb(225, 225, 225) : nk_rgb(175, 175, 175);
            context->style.contextual_button.hover       = i == GUIpitch ? nk_style_item_color(nk_rgb(245, 80, 85)) : nk_style_item_color(nk_rgb(40, 40, 40));
            context->style.contextual_button.text_hover  = i == GUIpitch ? nk_rgb(225, 225, 225) : nk_rgb(175, 175, 175);

            if (nk_combo_item_label(context, pitches[i], NK_TEXT_LEFT)) {
                GUIpitch = i;
            }
        }
        nk_combo_end(context);
    }
}

// ? MIGHT NEED WORK
void UI::resetButtonStyle() {
	context->style.button.normal      = nk_style_item_color(nk_rgb(50, 50, 50));
    context->style.button.text_normal = nk_rgb(175, 175, 175);
    context->style.button.hover       = nk_style_item_color(nk_rgb(40, 40, 40));
    context->style.button.text_hover  = nk_rgb(175, 175, 175);
}

// ? MIGHT NEED WORK
void UI::fslider(std::string label, float min, float max, float* value, float step) {
	float ratio = (*value - min) / (max - min);

    unsigned int r = ratio * 8 + 38;
    unsigned int g = ratio * 126 + 38;
    unsigned int b = ratio * 139 + 38;

    context->style.slider.bar_filled = nk_rgb(r, g, b);

    nk_layout_row_begin(context, NK_DYNAMIC, 15, 2);
    {
        nk_layout_row_push(context, 0.40f);
        nk_label(context, label.c_str(), NK_TEXT_LEFT);
        nk_layout_row_push(context, 0.60f);
        nk_slider_float(context, min, value, max, step);
    }
    nk_layout_row_end(context);
}

// ? MIGHT NEED WORK
void UI::islider(std::string label, int min, int max, int* value, int step) {
	float ratio = (float) (*value - min) / (float) (max - min);

    unsigned int r = ratio * 8 + 38;
    unsigned int g = ratio * 126 + 38;
    unsigned int b = ratio * 139 + 38;

    context->style.slider.bar_filled = nk_rgb(r, g, b);

    nk_layout_row_begin(context, NK_DYNAMIC, 15, 2);
    {
        nk_layout_row_push(context, 0.40f);
        nk_label(context, label.c_str(), NK_TEXT_LEFT);
        nk_layout_row_push(context, 0.60f);
        nk_slider_int(context, min, value, max, step);
    }
    nk_layout_row_end(context);
}

}
