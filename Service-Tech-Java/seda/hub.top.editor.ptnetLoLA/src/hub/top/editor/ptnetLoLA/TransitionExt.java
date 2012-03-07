/**
 *  Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 *  
 *  ServiceTechnolog.org - PetriNet Editor Framework
 *  
 *  This program and the accompanying materials are made available under
 *  the terms of the Eclipse Public License v1.0, which accompanies this
 *  distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
 *  
 *  Software distributed under the License is distributed on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 *  for the specific language governing rights and limitations under the
 *  License.
 * 
 *  The Original Code is this file as it was released on July 30, 2008.
 *  The Initial Developer of the Original Code is
 *  		Dirk Fahland
 *  
 *  Portions created by the Initial Developer are Copyright (c) 2008
 *  the Initial Developer. All Rights Reserved.
 * 
 *  Contributor(s):
 * 
 *  Alternatively, the contents of this file may be used under the terms of
 *  either the GNU General Public License Version 3 or later (the "GPL"), or
 *  the GNU Lesser General Public License Version 3 or later (the "LGPL"),
 *  in which case the provisions of the GPL or the LGPL are applicable instead
 *  of those above. If you wish to allow use of your version of this file only
 *  under the terms of either the GPL or the LGPL, and not to allow others to
 *  use your version of this file under the terms of the EPL, indicate your
 *  decision by deleting the provisions above and replace them with the notice
 *  and other provisions required by the GPL or the LGPL. If you do not delete
 *  the provisions above, a recipient may use your version of this file under
 *  the terms of any one of the EPL, the GPL or the LGPL.
 *
 * $Id$
 */
package hub.top.editor.ptnetLoLA;

import org.eclipse.emf.common.util.EList;


/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Transition Ext</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.editor.ptnetLoLA.TransitionExt#getProbability <em>Probability</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.TransitionExt#getMinTime <em>Min Time</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.TransitionExt#getCost <em>Cost</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.TransitionExt#getMaxTime <em>Max Time</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.TransitionExt#getConfidence <em>Confidence</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getTransitionExt()
 * @model
 * @generated
 */
public interface TransitionExt extends Transition {
  /**
   * Returns the value of the '<em><b>Probability</b></em>' attribute.
   * The default value is <code>"1.0"</code>.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Probability</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Probability</em>' attribute.
   * @see #setProbability(double)
   * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getTransitionExt_Probability()
   * @model default="1.0"
   * @generated
   */
  double getProbability();

  /**
   * Sets the value of the '{@link hub.top.editor.ptnetLoLA.TransitionExt#getProbability <em>Probability</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Probability</em>' attribute.
   * @see #getProbability()
   * @generated
   */
  void setProbability(double value);

		/**
   * Returns the value of the '<em><b>Min Time</b></em>' attribute.
   * The default value is <code>"0"</code>.
   * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Min Time</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
   * @return the value of the '<em>Min Time</em>' attribute.
   * @see #setMinTime(int)
   * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getTransitionExt_MinTime()
   * @model default="0" unique="false"
   * @generated
   */
	int getMinTime();

		/**
   * Sets the value of the '{@link hub.top.editor.ptnetLoLA.TransitionExt#getMinTime <em>Min Time</em>}' attribute.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @param value the new value of the '<em>Min Time</em>' attribute.
   * @see #getMinTime()
   * @generated
   */
	void setMinTime(int value);

		/**
   * Returns the value of the '<em><b>Cost</b></em>' attribute.
   * The default value is <code>"0"</code>.
   * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Cost</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
   * @return the value of the '<em>Cost</em>' attribute.
   * @see #setCost(double)
   * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getTransitionExt_Cost()
   * @model default="0" unique="false"
   * @generated
   */
	double getCost();

		/**
   * Sets the value of the '{@link hub.top.editor.ptnetLoLA.TransitionExt#getCost <em>Cost</em>}' attribute.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @param value the new value of the '<em>Cost</em>' attribute.
   * @see #getCost()
   * @generated
   */
	void setCost(double value);

		/**
   * Returns the value of the '<em><b>Max Time</b></em>' attribute.
   * The default value is <code>"0"</code>.
   * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Max Time</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
   * @return the value of the '<em>Max Time</em>' attribute.
   * @see #setMaxTime(int)
   * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getTransitionExt_MaxTime()
   * @model default="0"
   * @generated
   */
	int getMaxTime();

		/**
   * Sets the value of the '{@link hub.top.editor.ptnetLoLA.TransitionExt#getMaxTime <em>Max Time</em>}' attribute.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @param value the new value of the '<em>Max Time</em>' attribute.
   * @see #getMaxTime()
   * @generated
   */
	void setMaxTime(int value);

    /**
   * Returns the value of the '<em><b>Confidence</b></em>' attribute.
   * The literals are from the enumeration {@link hub.top.editor.ptnetLoLA.Confidence}.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of the '<em>Confidence</em>' attribute isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @return the value of the '<em>Confidence</em>' attribute.
   * @see hub.top.editor.ptnetLoLA.Confidence
   * @see #setConfidence(Confidence)
   * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getTransitionExt_Confidence()
   * @model
   * @generated
   */
  Confidence getConfidence();

    /**
   * Sets the value of the '{@link hub.top.editor.ptnetLoLA.TransitionExt#getConfidence <em>Confidence</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @param value the new value of the '<em>Confidence</em>' attribute.
   * @see hub.top.editor.ptnetLoLA.Confidence
   * @see #getConfidence()
   * @generated
   */
  void setConfidence(Confidence value);

} // TransitionExt
