from infra import renderer, image
from infra.core import RenderCase, VerificationResult

import inspect


class TestRunner:
    """
    Orchestrates the rendering and verification of test cases.
    """
    def run(self, case: RenderCase) -> VerificationResult:
        """
        Execute a test case: render the scene, save standard plots, and run all verifiers.
        @param case The test case to run.
        @return a combined VerificationResult. If multiple verifiers are used, they must all pass.
        """
        if case.get_output_dir() is None:
            caller_module = inspect.getmodule(inspect.currentframe().f_back)
            if caller_module is None:
                raise ValueError(f"cannot determine test module for RenderCase <{case.name}>")
            case.set_module_name(caller_module.__name__)

        case_output_dir = case.get_output_dir()
        case_output_dir.mkdir(parents=True, exist_ok=True)

        # Output file path (e.g., .../bvpt.pfm)
        output_path = case_output_dir / case.output_filename
        
        # 1. Render
        process = renderer.RenderProcess()
        process.set_scene_file_path(case.scene_path)
        process.set_image_output_path(output_path)
        process.set_num_render_threads(case.renderer_config.num_threads)
        process.set_image_format(case.renderer_config.output_format)
        if case.renderer_config.is_raw:
            process.request_raw_output()
        
        # Apply extra arguments
        for key, value in case.renderer_config.extra_args.items():
             process._set_argument(key, value)

        process.run_and_wait()

        # 2. Load result
        output_img = image.read_pfm(output_path)
        
        # 3. Verify (Run all verifiers)
        combined_passed = True
        combined_msg = []
        case.reset_report_state()

        for verifier_index, verifier in enumerate(case.verifiers):
            case.begin_verifier_report(type(verifier).__name__, verifier_index)
            verifier.save_report_artifacts(output_img, case_output_dir, case)
            result = verifier.verify(output_img, case_output_dir, case)
            if not result.passed:
                combined_passed = False
                if result.message:
                    combined_msg.append(result.message)
            case.end_verifier_report(result)

        case.sync_primary_report_fields()

        # 4. Save standard plot for report after metrics are available.
        output_img.save_plot(output_path, case.get_output_title())
        
        return VerificationResult(
            passed=combined_passed,
            message="; ".join(combined_msg),
            metrics={})
