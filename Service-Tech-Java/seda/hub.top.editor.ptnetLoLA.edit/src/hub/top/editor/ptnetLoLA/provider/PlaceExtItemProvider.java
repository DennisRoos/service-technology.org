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
package hub.top.editor.ptnetLoLA.provider;


import hub.top.editor.ptnetLoLA.PlaceExt;
import hub.top.editor.ptnetLoLA.PtnetLoLAPackage;

import java.util.Collection;
import java.util.List;

import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.edit.provider.ComposeableAdapterFactory;
import org.eclipse.emf.edit.provider.IEditingDomainItemProvider;
import org.eclipse.emf.edit.provider.IItemLabelProvider;
import org.eclipse.emf.edit.provider.IItemPropertyDescriptor;
import org.eclipse.emf.edit.provider.IItemPropertySource;
import org.eclipse.emf.edit.provider.IStructuredItemContentProvider;
import org.eclipse.emf.edit.provider.ITreeItemContentProvider;
import org.eclipse.emf.edit.provider.ItemPropertyDescriptor;
import org.eclipse.emf.edit.provider.ViewerNotification;

/**
 * This is the item provider adapter for a {@link hub.top.editor.ptnetLoLA.PlaceExt} object.
 * <!-- begin-user-doc -->
 * <!-- end-user-doc -->
 * @generated
 */
public class PlaceExtItemProvider
  extends PlaceItemProvider
  implements
    IEditingDomainItemProvider,
    IStructuredItemContentProvider,
    ITreeItemContentProvider,
    IItemLabelProvider,
    IItemPropertySource {
  /**
   * This constructs an instance from a factory and a notifier.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public PlaceExtItemProvider(AdapterFactory adapterFactory) {
    super(adapterFactory);
  }

  /**
   * This returns the property descriptors for the adapted class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public List<IItemPropertyDescriptor> getPropertyDescriptors(Object object) {
    if (itemPropertyDescriptors == null) {
      super.getPropertyDescriptors(object);

      addProbabilityPropertyDescriptor(object);
      addIsStartPropertyDescriptor(object);
    }
    return itemPropertyDescriptors;
  }

  /**
   * This adds a property descriptor for the Probability feature.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  protected void addProbabilityPropertyDescriptor(Object object) {
    itemPropertyDescriptors.add
      (createItemPropertyDescriptor
        (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
         getResourceLocator(),
         getString("_UI_PlaceExt_probability_feature"),
         getString("_UI_PropertyDescriptor_description", "_UI_PlaceExt_probability_feature", "_UI_PlaceExt_type"),
         PtnetLoLAPackage.Literals.PLACE_EXT__PROBABILITY,
         true,
         false,
         false,
         ItemPropertyDescriptor.REAL_VALUE_IMAGE,
         null,
         null));
  }

  /**
   * This adds a property descriptor for the Is Start feature.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated
   */
	protected void addIsStartPropertyDescriptor(Object object) {
    itemPropertyDescriptors.add
      (createItemPropertyDescriptor
        (((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
         getResourceLocator(),
         getString("_UI_PlaceExt_isStart_feature"),
         getString("_UI_PropertyDescriptor_description", "_UI_PlaceExt_isStart_feature", "_UI_PlaceExt_type"),
         PtnetLoLAPackage.Literals.PLACE_EXT__IS_START,
         true,
         false,
         false,
         ItemPropertyDescriptor.BOOLEAN_VALUE_IMAGE,
         null,
         null));
  }

		/**
   * This returns PlaceExt.gif.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public Object getImage(Object object) {
    return overlayImage(object, getResourceLocator().getImage("full/obj16/PlaceExt"));
  }

  /**
   * This returns the label text for the adapted class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public String getText(Object object) {
    String label = ((PlaceExt)object).getName();
    return label == null || label.length() == 0 ?
      getString("_UI_PlaceExt_type") :
      getString("_UI_PlaceExt_type") + " " + label;
  }

  /**
   * This handles model notifications by calling {@link #updateChildren} to update any cached
   * children and by creating a viewer notification, which it passes to {@link #fireNotifyChanged}.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public void notifyChanged(Notification notification) {
    updateChildren(notification);

    switch (notification.getFeatureID(PlaceExt.class)) {
      case PtnetLoLAPackage.PLACE_EXT__PROBABILITY:
      case PtnetLoLAPackage.PLACE_EXT__IS_START:
        fireNotifyChanged(new ViewerNotification(notification, notification.getNotifier(), false, true));
        return;
    }
    super.notifyChanged(notification);
  }

  /**
   * This adds {@link org.eclipse.emf.edit.command.CommandParameter}s describing the children
   * that can be created under this object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  protected void collectNewChildDescriptors(Collection<Object> newChildDescriptors, Object object) {
    super.collectNewChildDescriptors(newChildDescriptors, object);
  }

}
