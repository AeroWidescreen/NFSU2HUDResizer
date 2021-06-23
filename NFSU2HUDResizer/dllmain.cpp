#include "stdafx.h"
#include "stdio.h"
#include <windows.h>
#include "..\includes\injector\injector.hpp"
#include <cstdint>
#include "..\includes\IniReader.h"
#include <d3d9.h>

float TempHUDSize = 1.0f;
float TempHUDPos = 1.0f;
float HUDScale;
float HUDSize;
float HUDPos;
float FMVScale;
bool AutoHideCursor;

DWORD HUDSizeCodeCaveExit1 = 0x5C5026;
DWORD HUDSizeCodeCaveExit2 = 0x5C503D;
DWORD HUDSizeCodeCaveExit3 = 0x5C51DC;
DWORD DynoSizeCodeCaveExit = 0x50DD9F;
DWORD FMVSizeCodeCaveExit = 0x51A1C6;
DWORD ShowcaseMode = 0x836358;

void __declspec(naked) HUDSizeCodeCave()
{
	__asm {
		mov eax, dword ptr ds : [esp]
		cmp eax, 0x5C7EB4 // HUD
		je HUDSizeCodeCaveScale
		cmp eax, 0x5CC1CB // Mirror
		je HUDSizeCodeCaveScale
		cmp eax, 0x50B783 // Mouse
		je HUDSizeCodeCaveScale
		push ebp
		mov ebp, esp
		and esp, 0xFFFFFFF0
		jmp HUDSizeCodeCaveExit1

	HUDSizeCodeCaveScale:
		///////////////////////////////
		fld dword ptr ds : [TempHUDSize]
		fmul dword ptr ds : [HUDScale]
		fstp dword ptr ds : [HUDSize]
		fld dword ptr ds : [TempHUDPos]
		fsub dword ptr ds : [HUDSize]
		fstp dword ptr ds : [HUDPos]
		///////////////////////////////
		push ebp
		mov ebp, esp
		and esp, 0xFFFFFFF0
		sub esp, 0xD4
		mov eax, dword ptr ds : [0x8709A0]
		mov ecx, dword ptr ds : [ebp + 0x0C]
		push ebx
		xor bl, bl
		test eax, eax
		push esi
		mov esi, dword ptr ds : [ebp + 0x08]
		
		// Checks for Showcase Mode
		cmp dword ptr ds : [0x836358], 0x01
		jl HUDSizeCodeCaveFullscreenCheck1
		test eax, eax
		jmp HUDSizeCodeCaveExit2
	
	HUDSizeCodeCaveFullscreenCheck1:
		// Checks for Splash Screen
		cmp dword ptr ds : [esi + 0x04], 0x00000000
		jne HUDSizeCodeCaveFullscreenCheck2
		cmp dword ptr ds : [esi + 0x14], 0x00000000
		jne HUDSizeCodeCaveFullscreenCheck2
		cmp dword ptr ds : [esi + 0x24], 0x43F00000
		jne HUDSizeCodeCaveFullscreenCheck2
		cmp dword ptr ds : [esi + 0x34], 0x43F00000
		jne HUDSizeCodeCaveFullscreenCheck2
		test eax, eax
		jmp HUDSizeCodeCaveExit2

	HUDSizeCodeCaveFullscreenCheck2:
		// Checks for Contrast Overlay
		cmp dword ptr ds : [esi + 0x04], 0xC2AA0000
		jne HUDSizeCodeCaveNotFullscreen
		cmp dword ptr ds : [esi + 0x14], 0xC2AA0000
		jne HUDSizeCodeCaveNotFullscreen
		cmp dword ptr ds : [esi + 0x24], 0x440D4000
		jne HUDSizeCodeCaveNotFullscreen
		cmp dword ptr ds : [esi + 0x34], 0x440D4000
		jne	HUDSizeCodeCaveNotFullscreen
		jmp HUDSizeCodeCaveExit2

	HUDSizeCodeCaveNotFullscreen:
		test eax, eax
		push edi
		mov [esp + 0x1B], bl
		mov [esp + 0x1A], bl
		je HUDSizeCodeCaveScaleConditional1
		test ecx, ecx
		je HUDSizeCodeCaveScaleConditional2
		mov eax, [ecx + 0x24]
		cmp eax, 0x40CDC515
		je HUDSizeCodeCaveScaleConditional3
		cmp eax, 0x1149C7DA
		jne HUDSizeCodeCaveScaleConditional2

	HUDSizeCodeCaveScaleConditional3:
		mov bl, 0x01
		mov [esp + 0x1B], bl

	HUDSizeCodeCaveScaleConditional2:
		mov al, [esi + 0x71]
		cmp al, 0x72
		je HUDSizeCodeCaveScaleConditional4
		cmp al, 0x73
		jne HUDSizeCodeCaveScaleConditional1

	HUDSizeCodeCaveScaleConditional4:
		mov byte ptr [esp + 0x1A], 0x01

	HUDSizeCodeCaveScaleConditional1:
		fld dword ptr ds : [esi]
		mov al, [esi + 0x70]
		and al, 0x02
		fmul dword ptr ds : [0x79AC10]  // Width
		fmul dword ptr ds : [HUDSize] // Size
		fadd dword ptr ds : [HUDPos] // Pos
		fsub dword ptr ds : [0x784250]
		fstp dword ptr ds : [esp + 0x20]
		fld dword ptr ds : [esi + 0x04]
		fmul dword ptr ds : [0x79AC14] // Height
		fmul dword ptr ds : [HUDSize] // Size
		fadd dword ptr ds : [HUDPos] // Pos
		fsub dword ptr ds : [0x784250]
		fchs
		fstp dword ptr ds : [esp + 0x24]
		je HUDSizeCodeCaveScaleConditional5
		mov edx, dword ptr ds : [esi + 0x08]
		mov dword ptr ds : [esp + 0x28], edx
		jmp HUDSizeCodeCaveScaleConditional6

	HUDSizeCodeCaveScaleConditional5:
		mov dword ptr ds : [esp + 0x28], 0x3F800000
	
	HUDSizeCodeCaveScaleConditional6:
		fld dword ptr ds : [esi + 0x10]
		movzx edi, byte ptr[esi + 0x61]
		fmul dword ptr ds : [0x79AC10] // Width
		fmul dword ptr ds : [HUDSize] // Size
		fadd dword ptr ds : [HUDPos] // Pos
		xor edx, edx
		mov dh, [esi + 0x63]
		mov dword ptr ds : [esp + 0x38], 0x3F800000
		fsub dword ptr ds : [0x784250]
		mov dword ptr ds : [esp + 0x3C], 0x3F800000
		fstp dword ptr ds : [esp + 0x40]
		fld dword ptr ds : [esi + 0x14]
		fmul dword ptr ds : [0x79AC14] // Height
		fmul dword ptr ds : [HUDSize] // Size
		fadd dword ptr ds : [HUDPos] // Pos
		mov dl, [esi + 0x60]
		fsub dword ptr ds : [0x784250]
		fchs
		fstp dword ptr ds : [esp + 0x44]
		shl edx, 0x08
		or edx, edi
		movzx edi, byte ptr [esi + 0x62]
		shl edx, 0x08
		or edx, edi
		test al, al
		mov dword ptr ds : [esp + 0x2C], edx
		mov edx, dword ptr ds : [esi + 0x40]
		mov dword ptr ds : [esp + 0x30], edx
		mov edx, dword ptr ds : [esi + 0x44]
		mov dword ptr ds : [esp + 0x34], edx
		je HUDSizeCodeCaveScaleConditional7
		mov edx, dword ptr ds : [esi + 0x18]
		mov dword ptr ds : [esp + 0x48], edx
		jmp HUDSizeCodeCaveScaleConditional8
	
	HUDSizeCodeCaveScaleConditional7:
		mov dword ptr ds : [esp + 0x48], 0x3F800000

	HUDSizeCodeCaveScaleConditional8:
		fld dword ptr ds : [esi + 0x20]
		movzx edi, byte ptr [esi + 0x65]
		fmul dword ptr ds : [0x79AC10] // Width
		fmul dword ptr ds : [HUDSize] // Size
		fadd dword ptr ds : [HUDPos] // Pos
		xor edx, edx
		mov dh, [esi + 0x67]
		mov dword ptr ds : [esp + 0x58], 0x3F800000
		fsub dword ptr ds : [0x784250]
		mov dword ptr ds : [esp + 0x5C], 0x3F800000
		fstp dword ptr ds : [esp + 0x60]
		fld dword ptr ds : [esi + 0x24]
		fmul dword ptr ds : [0x79AC14] // Height
		fmul dword ptr ds : [HUDSize] // Size
		fadd dword ptr ds : [HUDPos] // Pos
		mov dl, [esi + 0x64]
		fsub dword ptr ds : [0x784250]
		fchs
		fstp dword ptr ds : [esp + 0x64]
		shl edx, 0x08
		or edx, edi
		movzx edi, byte ptr [esi + 0x66]
		shl edx, 0x08
		or edx, edi
		test al, al
		mov dword ptr ds : [esp + 0x4C], edx
		mov edx, dword ptr ds : [esi + 0x48]
		mov dword ptr ds : [esp + 0x50], edx
		mov edx, dword ptr ds : [esi + 0x4C]
		mov dword ptr ds : [esp + 0x54], edx
		je HUDSizeCodeCaveScaleConditional9
		mov edx, dword ptr ds : [esi + 0x28]
		mov dword ptr ds : [esp + 0x68], edx
		jmp HUDSizeCodeCaveScaleConditional10

	HUDSizeCodeCaveScaleConditional9:
		mov dword ptr ds : [esp + 0x68], 0x3F800000

	HUDSizeCodeCaveScaleConditional10:
		fld dword ptr ds : [esi + 0x30]
		movzx edi, byte ptr [esi + 0x69]
		fmul dword ptr ds : [0x79AC10] // Width
		fmul dword ptr ds : [HUDSize] // Size
		fadd dword ptr ds : [HUDPos] // Pos
		xor edx, edx
		mov dh, [esi + 0x6B]
		mov dword ptr ds : [esp + 0x78], 0x3F8000000
		fsub dword ptr ds : [0x784250]
		mov dword ptr ds : [esp + 0x7C], 0x3F800000
		fstp dword ptr ds: [esp + 0x80]
		fld dword ptr ds : [esi + 0x34]
		fmul dword ptr ds : [0x79AC14] // Height
		fmul dword ptr ds : [HUDSize] // Size
		fadd dword ptr ds : [HUDPos] // Pos
		jmp HUDSizeCodeCaveExit3
	}
}

void __declspec(naked) DynoSizeCodeCave()
{
	__asm {
		mov dword ptr ds : [esp + 0x24], 0x00000000
		fld dword ptr ds : [0x79AC10] // Width
		fmul dword ptr ds : [HUDSize] // Size
		fadd dword ptr ds : [HUDPos] // Pos
		mov dword ptr ds : [esp + 0x28], edi
		mov dword ptr ds : [esp + 0x2C], 0x3F000000
		fmul st(0), st(1)
		mov dword ptr ds : [esp + 0x30], 0x3F000000
		fld dword ptr ds : [ebx]
		mov dword ptr ds : [esp + 0x44], 0x00000000
		fmul dword ptr ds : [0x79AC10] // Width
		fmul dword ptr ds : [HUDSize] // Size
		fadd dword ptr ds : [HUDPos] // Pos
		mov dword ptr ds : [esp + 0x48], edi
		mov dword ptr ds : [esp + 0x4C], 0x3F000000
		mov dword ptr ds : [esp + 0x50], 0x3F000000
		fadd st(0), st(1)
		fsub dword ptr ds : [0x784250]
		fstp dword ptr ds : [esp + 0x1C]
		fxch st(1)
		fmul dword ptr ds : [0x79AC14] // Height
		fmul dword ptr ds : [HUDSize] // Size
		fadd dword ptr ds : [HUDPos] // Pos
		fstp dword ptr ds : [esp + 0x18]
		fld dword ptr ds : [ebx + 0x04]
		fmul dword ptr ds : [0x79AC14] // Height
		fmul dword ptr ds : [HUDSize] // Size
		fadd dword ptr ds : [esp + 0x18]
		fsub dword ptr ds : [0x784250]
		fchs
		fstp dword ptr ds : [esp + 0x20]
		fld dword ptr ds : [ebp + 0x00]
		fmul dword ptr ds : [0x79AC10] // Width
		fmul dword ptr ds : [HUDSize] // Size
		fadd dword ptr ds : [HUDPos] // Pos
		fadd st(0), st(1)
		fsub dword ptr ds : [0x784250]
		fstp dword ptr ds : [esp + 0x3C]
		fstp st(0)
		fld dword ptr ds : [ebp + 0x04]
		fmul dword ptr ds : [0x79AC14] // Height
		fmul dword ptr ds : [HUDSize] // Size
		jmp DynoSizeCodeCaveExit
	}
}

void __declspec(naked) FMVSizeCodeCave()
{
	__asm {
		mov eax, dword ptr ds : [esp]
		cmp eax, 0x536A39 // FMVs
		je FMVSizeCodeCaveScale
		push ebp
		mov ebp, esp
		and esp, 0xFFFFFFF0
		jmp FMVSizeCodeCaveExit

	FMVSizeCodeCaveScale:
		///////////////////////////////
		fld dword ptr ds : [esp+04]
		fdiv dword ptr ds : [HUDSize] // Height
		fmul dword ptr ds : [FMVScale] // Scale
		fstp dword ptr ds : [esp+04]
		fld dword ptr ds : [esp + 0x08]
		fdiv dword ptr ds : [HUDSize] // Width
		fmul dword ptr ds : [FMVScale] // Scale
		fstp dword ptr ds : [esp + 0x08]
		fld dword ptr ds : [esp + 0x0C]
		fdiv dword ptr ds : [HUDSize] // Width
		fmul dword ptr ds : [FMVScale] // Scale
		fstp dword ptr ds : [esp + 0x0C]
		fld dword ptr ds : [esp + 0x10]
		fdiv dword ptr ds : [HUDSize] // Height
		fmul dword ptr ds : [FMVScale] // Scale
		fstp dword ptr ds : [esp + 0x10]
		///////////////////////////////
		push ebp
		mov ebp, esp
		and esp, 0xFFFFFFF0
		jmp FMVSizeCodeCaveExit
	}
}


void Init()
{
	// Read values from .ini
	CIniReader iniReader("NFSU2HUDResizer.ini");

	// General
	HUDScale = iniReader.ReadFloat("GENERAL", "HUDScale", 0.92f);
	FMVScale = iniReader.ReadFloat("GENERAL", "FMVScale", 1.0f);
	AutoHideCursor = iniReader.ReadInteger("GENERAL", "AutoHideCursor", 1);

	{
		// HUD Size
		injector::MakeJMP(0x5C5020, HUDSizeCodeCave, true);
		// Dyno Chart Size
		injector::MakeJMP(0x50DD06, DynoSizeCodeCave, true);
		// FMV SIze
		injector::MakeJMP(0x51A1C0, FMVSizeCodeCave, true);
	}

	if (AutoHideCursor)
	{
		// jmp 0x50B786
		injector::WriteMemory<uint32_t>(0x50B4C6, 0x0002BBE9, true);
		injector::WriteMemory<uint8_t>(0x50B4CA, 0x00, true);
		injector::MakeNOP(0x50B4CB, 1, true);
	}
}
	

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		uintptr_t base = (uintptr_t)GetModuleHandleA(NULL);
		IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)(base);
		IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);

		if ((base + nt->OptionalHeader.AddressOfEntryPoint + (0x400000 - base)) == 0x75BCC7) // Check if .exe file is compatible - Thanks to thelink2012 and MWisBest
			Init();

		else
		{
			MessageBoxA(NULL, "This .exe is not supported.\nPlease use v1.2 NTSC speed2.exe (4,57 MB (4.800.512 bytes)).", "NFSU2 HUD Resizer by Aero_", MB_ICONERROR);
			return FALSE;
		}
	}
	return TRUE;

}