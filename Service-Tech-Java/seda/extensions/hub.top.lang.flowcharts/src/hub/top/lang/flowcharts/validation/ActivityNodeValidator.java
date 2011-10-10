/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.lang.flowcharts.validation;

import hub.top.lang.flowcharts.ResourceNode;

import org.eclipse.emf.common.util.EList;

/**
 * A sample validator interface for {@link hub.top.lang.flowcharts.ActivityNode}.
 * This doesn't really do anything, and it's not a real EMF artifact.
 * It was generated by the org.eclipse.emf.examples.generator.validator plug-in to illustrate how EMF's code generator can be extended.
 * This can be disabled with -vmargs -Dorg.eclipse.emf.examples.generator.validator=false.
 */
public interface ActivityNodeValidator {
	boolean validate();

	boolean validateActivityOutput(EList<ResourceNode> value);
	boolean validateActivityOutputOpt(EList<ResourceNode> value);
	boolean validateOptional(boolean value);

	boolean validateActivityInputOpt(EList<ResourceNode> value);

	boolean validateActivityInput(EList<ResourceNode> value);
}