# Spec: AgentPlanning Profile Workflow

## Objective

Make the repository's `plan-agent` a planning-only coordinator that turns a feature request or existing plan into a reviewed, implementation-ready specification. Its audience is engineers and implementation agents that need a clear, approved handoff rather than inferred requirements.

Success means every finalized plan is grounded in the repository, states its assumptions, passes mandatory plan review, is approved by the human requester, is displayed in chat, and is saved under `specs/`.

## Tech Stack

- Agent profiles: TOML in `.agents/`
- Specifications: Markdown in `specs/`
- Application baseline: C++ compiled by `g++`
- Profile validation: Python 3 standard-library `tomllib`

No new dependencies, runtime services, or application behavior are introduced.

## Commands

```bash
./test_runner.sh
git diff --check
python3 -c "import tomllib; from pathlib import Path; [tomllib.loads(path.read_text()) for path in (Path('.agents/plan-agent.toml'), Path('.agents/plan-reviewer.toml'))]; print('TOML profiles parse successfully')"
```

## Project Structure

```text
.agents/                   Agent role definitions and local skills
.agents/plan-agent.toml    Authoritative planning-agent instructions
.agents/plan-reviewer.toml Authoritative review-gate instructions
specs/                     Approved implementation-ready specifications
tests/                     Application test code
main.cpp                   C++ application entry point
test_runner.sh             Baseline build-and-run script
```

## Code Style

Profile instructions use concise, imperative Markdown with explicit gates and boundaries. Specifications use descriptive headings, repository-backed facts, and unambiguous commands.

```markdown
## Boundaries

- Always: inspect relevant files and include executable validation commands.
- Ask first: add dependencies or modify CI, schemas, or security behavior.
- Never: implement product code while producing a plan.
```

## Testing Strategy

- Parse each profile with Python's standard-library TOML parser.
- Run whitespace validation with `git diff --check`.
- Run the existing C++ build-and-run script to confirm the profile/documentation change does not disturb the project baseline.
- Manually exercise the documented workflow with complete, ambiguous, UI-affecting, and approval-gated feature requests.

## Boundaries

- **Always:** inspect the repository before planning, surface material assumptions, require plan-reviewer review, and validate changed profiles.
- **Ask first:** add dependencies, change schemas or public interfaces, modify CI or deployment, alter security or data retention, or make destructive changes.
- **Never:** commit secrets, modify product code as part of planning, remove tests to force success, or finalize a plan with material ambiguity.

## Success Criteria

1. `.agents/plan-agent.toml` and `.agents/plan-reviewer.toml` parse as TOML and declare themselves as the repository source of truth.
2. The plan-agent follows discovery, requirements, draft, mandatory review, human approval, and delivery gates.
3. A final plan contains objectives, commands, structure, code style, testing, boundaries, success criteria, risks, tasks, and validation commands.
4. UI or user-interaction plans include E2E test work and validation.
5. Approved plans are shown in chat and saved under `specs/` with descriptive unique filenames.

## Open Questions

None. The TOML profiles are intentionally maintained directly because no source-file or generation workflow exists in this repository.
