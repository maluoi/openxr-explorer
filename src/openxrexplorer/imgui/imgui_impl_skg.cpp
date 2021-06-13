#include "imgui_impl_skg.h"
#include "imgui_shader.hlsl.h"

#define SKG_IMPL
#include "sk_gpu.h"

int32_t      im_vb_size = 0;
skg_vert_t  *im_vb_data = nullptr;
skg_buffer_t im_vb      = {};
int32_t      im_ib_size = 0;
uint32_t    *im_ib_data = nullptr;
skg_buffer_t im_ib      = {};
skg_mesh_t   im_mesh    = {};

skg_tex_t      im_font_tex    = {};
skg_buffer_t   im_shader_vars = {};
skg_shader_t   im_shader      = {};
skg_pipeline_t im_pipeline    = {};

struct VERTEX_CONSTANT_BUFFER {
	float mvp[4][4];
};

// Render function
void ImGui_ImplSkg_RenderDrawData(ImDrawData* draw_data) {
	// Avoid rendering when minimized
	if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
		return;

	while (im_vb_size <= draw_data->TotalVtxCount) {
		im_vb_size += 5000;
		skg_buffer_destroy(&im_vb);
		im_vb = skg_buffer_create(nullptr, im_vb_size, sizeof(skg_vert_t), skg_buffer_type_vertex, skg_use_dynamic);
		free(im_vb_data);
		im_vb_data = (skg_vert_t*)malloc(im_vb_size * sizeof(skg_vert_t));

		skg_mesh_set_verts(&im_mesh, &im_vb);
	}
	while (im_ib_size <= draw_data->TotalIdxCount) {
		im_ib_size += 10000;
		skg_buffer_destroy(&im_ib);
		im_ib = skg_buffer_create(nullptr, im_ib_size, sizeof(uint32_t), skg_buffer_type_index, skg_use_dynamic);
		free(im_ib_data);
		im_ib_data = (uint32_t*)malloc(im_ib_size * sizeof(uint32_t));

		skg_mesh_set_inds(&im_mesh, &im_ib);
	}

	// Upload vertex/index data into a single contiguous GPU buffer
	skg_vert_t* vtx_dst = im_vb_data;
	uint32_t*   idx_dst = im_ib_data;
	for (int n = 0; n < draw_data->CmdListsCount; n++) {
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		for (int i = 0; i < cmd_list->VtxBuffer.Size; i++) {
			skg_vert_t *v   = &vtx_dst[i];
			ImDrawVert *src = &cmd_list->VtxBuffer.Data[i];
			v->pos[0] = src->pos[0];
			v->pos[1] = src->pos[1];
			v->uv [0] = src->uv [0];
			v->uv [1] = src->uv [1];
			memcpy(&v->col, &src->col, sizeof(ImU32));
		}
		for (int i = 0; i < cmd_list->IdxBuffer.Size; i++) {
			idx_dst[i] = cmd_list->IdxBuffer.Data[i];
		}
		//memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		//memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		vtx_dst += cmd_list->VtxBuffer.Size;
		idx_dst += cmd_list->IdxBuffer.Size;
	}
	skg_buffer_set_contents(&im_vb, im_vb_data, (vtx_dst - im_vb_data) * sizeof(skg_vert_t));
	skg_buffer_set_contents(&im_ib, im_ib_data, (idx_dst - im_ib_data) * sizeof(uint32_t  ));

	// Setup orthographic projection matrix into our constant buffer
	// Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
	{
		float L = draw_data->DisplayPos.x;
		float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
		float T = draw_data->DisplayPos.y;
		float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
		float mvp[4][4] = {
			{ 2.0f/(R-L),  0.0f,        0.0f, 0.0f },
			{ 0.0f,        2.0f/(T-B),  0.0f, 0.0f },
			{ 0.0f,        0.0f,        0.5f, 0.0f },
			{ (R+L)/(L-R), (T+B)/(B-T), 0.5f, 1.0f }, };
		skg_buffer_set_contents(&im_shader_vars, mvp, sizeof(mvp));
	}

	skg_pipeline_bind(&im_pipeline);
	skg_buffer_bind(&im_shader_vars, { 0, skg_stage_vertex }, 0);
	skg_mesh_bind(&im_mesh);

	// Render command lists
	// (Because we merged all buffers into a single one, we maintain our own offset into them)
	int global_idx_offset = 0;
	int global_vtx_offset = 0;
	ImVec2 clip_off = draw_data->DisplayPos;
	for (int n = 0; n < draw_data->CmdListsCount; n++) {
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback != NULL) {
				// User callback, registered via ImDrawList::AddCallback()
				// (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
				if (pcmd->UserCallback != ImDrawCallback_ResetRenderState)
					pcmd->UserCallback(cmd_list, pcmd);
			} else {
				// Apply scissor/clipping rectangle
				int32_t rect[4] = {
					pcmd->ClipRect.x - clip_off.x,
					pcmd->ClipRect.y - clip_off.y,
					pcmd->ClipRect.z - pcmd->ClipRect.x,
					pcmd->ClipRect.w - pcmd->ClipRect.y};
				skg_scissor(rect);
				
				skg_tex_bind((skg_tex_t*)pcmd->TextureId, skg_bind_t{ 0, skg_stage_pixel });
				skg_draw(pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, pcmd->ElemCount, 1);
			}
		}
		global_idx_offset += cmd_list->IdxBuffer.Size;
		global_vtx_offset += cmd_list->VtxBuffer.Size;
	}
}

bool ImGui_ImplSkg_Init() {
	im_shader      = skg_shader_create_memory(sks_imgui_shader_hlsl, sizeof(sks_imgui_shader_hlsl));
	im_pipeline    = skg_pipeline_create(&im_shader);
	im_shader_vars = skg_buffer_create(nullptr, 1, sizeof(float[4][4]), skg_buffer_type_constant, skg_use_dynamic);
	im_mesh        = skg_mesh_create(nullptr, nullptr);

	skg_pipeline_set_cull        (&im_pipeline, skg_cull_none);
	skg_pipeline_set_transparency(&im_pipeline, skg_transparency_blend);
	skg_pipeline_set_depth_write (&im_pipeline, false);
	skg_pipeline_set_depth_test  (&im_pipeline, skg_depth_test_always);
	skg_pipeline_set_scissor     (&im_pipeline, true);

	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	void*    pixels;
	int      width, height;
	io.Fonts->GetTexDataAsRGBA32((unsigned char **)&pixels, &width, &height);

	im_font_tex = skg_tex_create(skg_tex_type_image, skg_use_static, skg_tex_fmt_rgba32_linear, skg_mip_none);
	skg_tex_set_contents(&im_font_tex, pixels, width, height);

	io.Fonts->TexID = (ImTextureID)&im_font_tex;
	return true;
}

void ImGui_ImplSkg_Shutdown() {
	skg_buffer_destroy(&im_shader_vars);
	skg_buffer_destroy(&im_vb);
	skg_buffer_destroy(&im_ib);

	skg_pipeline_destroy(&im_pipeline);
	skg_shader_destroy  (&im_shader);

	skg_tex_destroy(&im_font_tex);
}