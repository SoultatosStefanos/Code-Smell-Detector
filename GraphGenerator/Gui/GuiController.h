// GUI-Clang bridge.
// Soultatos Stefanos 2022

#pragma once

#include <functional>

#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "Gui.h"

namespace gui {

	class GuiController final {
	public:
		using FilePred = std::function<bool(const char*)>;
		using MatchResult = clang::ast_matchers::MatchFinder::MatchResult;
		using Decl = clang::Decl;

		GuiController(const GuiController&) = delete;
		GuiController(GuiController&&) = delete;

		GuiController& operator=(const GuiController&) = delete;	
		GuiController& operator=(GuiController&&) = delete;		

		static GuiController& GetSingleton();

		void SetIgnoredPred(const FilePred& f) { m_is_ignored = f; }
		void SetHeaderPred(const FilePred& f) { m_is_header = f; }

		void UpdateGui(const MatchResult& res, const Decl& decl) const;

	private:
		GuiController() = default;
		~GuiController() = default;

		FilePred m_is_ignored;
		FilePred m_is_header;
	};

} // gui