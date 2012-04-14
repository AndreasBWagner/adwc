/*
 * Copyright © 2011 Intel Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  The copyright holders make
 * no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <unistd.h>
#include <fcntl.h>

#include "adwc.h"
/*
WL_EXPORT void
weston_spring_init(struct weston_spring *spring,
		 double k, double current, double target)
{
	spring->k = k;
	spring->friction = 400.0;
	spring->current = current;
	spring->previous = current;
	spring->target = target;
}

WL_EXPORT void
weston_spring_update(struct weston_spring *spring, uint32_t msec)
{
	double force, v, current, step;

	step = 0.01;
	while (4 < msec - spring->timestamp) {
		current = spring->current;
		v = current - spring->previous;
		force = spring->k * (spring->target - current) / 10.0 +
			(spring->previous - current) - v * spring->friction;

		spring->current =
			current + (current - spring->previous) +
			force * step * step;
		spring->previous = current;

#if 0
		if (spring->current >= 1.0) {
#ifdef TWEENER_BOUNCE
			spring->current = 2.0 - spring->current;
			spring->previous = 2.0 - spring->previous;
#else
			spring->current = 1.0;
			spring->previous = 1.0;
#endif
		}

		if (spring->current <= 0.0) {
			spring->current = 0.0;
			spring->previous = 0.0;
		}
#endif
		spring->timestamp += 4;
	}
}

WL_EXPORT int
weston_spring_done(struct weston_spring *spring)
{
	return fabs(spring->previous - spring->target) < 0.0002 &&
		fabs(spring->current - spring->target) < 0.0002;
}

struct weston_zoom {
	struct weston_surface *surface;
	struct weston_animation animation;
	struct weston_spring spring;
	struct weston_transform transform;
	struct wl_listener listener;
	GLfloat start, stop;
	void (*done)(struct weston_zoom *zoom, void *data);
	void *data;
};
*/
/*
static void
weston_zoom_destroy(struct weston_zoom *zoom)
{
	wl_list_remove(&zoom->animation.link);
	wl_list_remove(&zoom->listener.link);
	wl_list_remove(&zoom->transform.link);
	zoom->surface->geometry.dirty = 1;
	if (zoom->done)
		zoom->done(zoom, zoom->data);
	free(zoom);
}

static void
handle_zoom_surface_destroy(struct wl_listener *listener, void *data)
{
	struct weston_zoom *zoom =
		container_of(listener, struct weston_zoom, listener);

	weston_zoom_destroy(zoom);
}

static void
weston_zoom_frame(struct weston_animation *animation,
		struct weston_output *output, uint32_t msecs)
{
	struct weston_zoom *zoom =
		container_of(animation, struct weston_zoom, animation);
	struct weston_surface *es = zoom->surface;
	GLfloat scale;

	weston_spring_update(&zoom->spring, msecs);

	if (weston_spring_done(&zoom->spring)) {
		weston_zoom_destroy(zoom);
		return;
	}

	scale = zoom->start +
		(zoom->stop - zoom->start) * zoom->spring.current;
	weston_matrix_init(&zoom->transform.matrix);
	weston_matrix_translate(&zoom->transform.matrix,
				-0.5f * es->geometry.width,
				-0.5f * es->geometry.height, 0);
	weston_matrix_scale(&zoom->transform.matrix, scale, scale, scale);
	weston_matrix_translate(&zoom->transform.matrix,
				0.5f * es->geometry.width,
				0.5f * es->geometry.height, 0);

	es->alpha = zoom->spring.current * 255;
	if (es->alpha > 255)
		es->alpha = 255;

	zoom->surface->geometry.dirty = 1;
	weston_compositor_schedule_repaint(es->compositor);
}

WL_EXPORT struct weston_zoom *
weston_zoom_run(struct weston_surface *surface, GLfloat start, GLfloat stop,
	      weston_zoom_done_func_t done, void *data)
{
	struct weston_zoom *zoom;

	zoom = malloc(sizeof *zoom);
	if (!zoom)
		return NULL;

	zoom->surface = surface;
	zoom->done = done;
	zoom->data = data;
	zoom->start = start;
	zoom->stop = stop;
	wl_list_insert(&surface->geometry.transformation_list,
		       &zoom->transform.link);
	weston_spring_init(&zoom->spring, 200.0, 0.0, 1.0);
	zoom->spring.friction = 700;
	zoom->spring.timestamp = weston_compositor_get_time();
	zoom->animation.frame = weston_zoom_frame;
	weston_zoom_frame(&zoom->animation, NULL, zoom->spring.timestamp);

	zoom->listener.notify = handle_zoom_surface_destroy;
	wl_signal_add(&surface->surface.resource.destroy_signal,
		      &zoom->listener);

	wl_list_insert(&surface->compositor->animation_list,
		       &zoom->animation.link);

	return zoom;
}
*/
struct weston_binding {
	uint32_t key;
	uint32_t button;
	uint32_t axis;
	uint32_t modifier;
	weston_binding_handler_t handler;
	void *data;
	struct wl_list link;
};

WL_EXPORT struct weston_binding *
weston_compositor_add_binding(struct weston_compositor *compositor,
			    uint32_t key, uint32_t button, uint32_t axis, uint32_t modifier,
			    weston_binding_handler_t handler, void *data)
{
	struct weston_binding *binding;

	binding = malloc(sizeof *binding);
	if (binding == NULL)
		return NULL;

	binding->key = key;
	binding->button = button;
	binding->axis = axis;
	binding->modifier = modifier;
	binding->handler = handler;
	binding->data = data;
	wl_list_insert(compositor->binding_list.prev, &binding->link);

	return binding;
}

WL_EXPORT void
weston_binding_destroy(struct weston_binding *binding)
{
	wl_list_remove(&binding->link);
	free(binding);
}

WL_EXPORT void
weston_binding_list_destroy_all(struct wl_list *list)
{
	struct weston_binding *binding, *tmp;

	wl_list_for_each_safe(binding, tmp, list, link)
		weston_binding_destroy(binding);
}

struct binding_keyboard_grab {
	uint32_t key;
	struct wl_keyboard_grab grab;
};

static void
binding_key(struct wl_keyboard_grab *grab,
	    uint32_t time, uint32_t key, int32_t state)
{
	struct binding_keyboard_grab *b =
		container_of(grab, struct binding_keyboard_grab, grab);
	struct wl_resource *resource;
	struct wl_display *display;
	uint32_t serial;

	resource = grab->input_device->keyboard_focus_resource;
	if (key == b->key) {
		if (!state) {
			wl_input_device_end_keyboard_grab(grab->input_device);
			free(b);
		}
	} else if (resource) {
		display = wl_client_get_display(resource->client);
		serial = wl_display_next_serial(display);
		wl_input_device_send_key(resource, serial, time, key, state);
	}
}

static const struct wl_keyboard_grab_interface binding_grab = {
	binding_key
};

static void
install_binding_grab(struct wl_input_device *device,
		     uint32_t time, uint32_t key)
{
	struct binding_keyboard_grab *grab;

	grab = malloc(sizeof *grab);
	grab->key = key;
	grab->grab.interface = &binding_grab;
	wl_input_device_start_keyboard_grab(device, &grab->grab);
}

WL_EXPORT void
weston_compositor_run_binding(struct weston_compositor *compositor,
			      struct weston_input_device *device,
			      uint32_t time, uint32_t key,
			      uint32_t button, uint32_t axis, int32_t state)
{
	struct weston_binding *b;

	wl_list_for_each(b, &compositor->binding_list, link) {
		if (b->key == key && b->button == button && b->axis == axis &&
		    b->modifier == device->modifier_state && state) {
			b->handler(&device->input_device,
				   time, key, button, axis, state, b->data);

			/* If this was a key binding and it didn't
			 * install a keyboard grab, install one now to
			 * swallow the key release. */
			if (b->key &&
			    device->input_device.keyboard_grab ==
			    &device->input_device.default_keyboard_grab)
				install_binding_grab(&device->input_device,
						     time, key);
		}
	}
}

WL_EXPORT int
weston_environment_get_fd(const char *env)
{
	char *e, *end;
	int fd, flags;

	e = getenv(env);
	if (!e)
		return -1;
	fd = strtol(e, &end, 0);
	if (*end != '\0')
		return -1;

	flags = fcntl(fd, F_GETFD);
	if (flags == -1)
		return -1;

	fcntl(fd, F_SETFD, flags | FD_CLOEXEC);
	unsetenv(env);

	return fd;
}
