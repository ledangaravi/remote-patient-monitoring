/*
 Copyright 2018-present the Material Components for iOS authors. All Rights Reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

import UIKit
import CoreBluetooth
import Charts
import PusherSwift
import Alamofire
import MaterialComponents
let hearRateserviceCBUUID = CBUUID(string: "0x180D") // heart rate UUID
let batteryServiceCBUUID = CBUUID(string: "0x180F") //battery service UUID
let healthThermometerCBUUID = CBUUID(string: "0x1809")
let heartRateMeasurementCharacteristicCBUUID = CBUUID(string: "2A37")
let bodySensorLocationCharacteristicCBUUID = CBUUID(string: "2A38")
let batteryLevelCBUUID = CBUUID(string: "2A19")
let tempMeasuermentCBUUID = CBUUID(string: "2A1C")
let P24ServiceCBUUID = CBUUID(string: "34D60724-E9CE-476C-BAD6-FB0F310898E1")
let stepCountCharacteristicCBUUID = CBUUID(string: "34D651E7-E9CE-476C-BAD6-FB0F310898E1")
let bloodOxygenCharacteristicCBUUID = CBUUID(string: "34D60202-E9CE-476C-BAD6-FB0F310898E1")
var devices:[String] = []
var peripherals:[CBPeripheral] = []
var fvalues: [Int] = [80, 81, 80, 82, 81, 80, 81, 82]
var i = -1

protocol SendManager {
    func send(str:String, str2:String)
}


class HomeViewController: UICollectionViewController {
    var delegate:SendManager?
    var centralManager: CBCentralManager! //adding central manager
    var heartRatePeripheral: CBPeripheral! //adding heart rate peripheral
    var shouldDisplayLogin = true
    var appBarViewController = MDCAppBarViewController()
    ////////////////////*Expanded Variables*/////////////////////////
    var cellWidth:CGFloat{
        return CGFloat(359)    }
    var expandedHeight : CGFloat = 400
    var notExpandedHeight : CGFloat = 140
    var dataSource = ["data0","data1","data2","data3","data4"]
    var isExpanded = [Bool]()
    /////////////////////////////////////////////
    
    
  override func viewDidLoad() {
    if let first = self.tabBarController?.viewControllers?[0] as? alerHistoryR {
        self.delegate = first
    }
    self.delegate = alerts
    super.viewDidLoad()
     isExpanded = Array(repeating: false, count: Catalog.count)
    self.view.tintColor = .white
    self.view.backgroundColor = .white
    centralManager = CBCentralManager(delegate: self, queue: nil)
    self.title = "Patient24"
    self.collectionView?.backgroundColor = .white
    let titleDict: NSDictionary = [NSAttributedStringKey.foregroundColor: UIColor.white]
    self.appBarViewController.navigationBar.titleTextAttributes = (titleDict as! [NSAttributedStringKey : Any])

    // AppBar Setup
    //TODO: Add the appBar controller and views
    self.addChildViewController(self.appBarViewController)
    self.view.addSubview(self.appBarViewController.view)
    self.appBarViewController.didMove(toParentViewController: self)
    

    // Set the tracking scroll view.
    self.appBarViewController.headerView.trackingScrollView = self.collectionView

    // Setup Navigation Items
    //TODO: Create the items and set them on the view controller's navigationItems properties
    let menuItemImage = UIImage(named: "MenuItem")
    let templatedMenuItemImage = menuItemImage?.withRenderingMode(.alwaysTemplate)
    let menuItem = UIBarButtonItem(image: templatedMenuItemImage,
                                   style: .plain,
                                   target: self,
                                   action: #selector(menuItemTapped(sender:)))
    self.navigationItem.leftBarButtonItem = menuItem
   
    let bluetoothitem = UIImage(named: "bluetooth")
    let templatedSearchItemImage = bluetoothitem?.withRenderingMode(.alwaysTemplate)
    let searchItem = UIBarButtonItem(image: templatedSearchItemImage,
                                     style: .plain,
                                     target: nil,
                                     action: #selector(bluetoothtapped(sender:)))
    /*
 
    let tuneItemImage = UIImage(named: "TuneItem")
    let templatedTuneItemImage = tuneItemImage?.withRenderingMode(.alwaysTemplate)
    let tuneItem = UIBarButtonItem(image: templatedTuneItemImage,
                                   style: .plain,
                                   target: nil,
                                   action: nil)
  */
    self.navigationItem.rightBarButtonItem = searchItem

    
    //***************************COLOR STUFF:*****************************//
    self.view.backgroundColor =  .white
    self.collectionView?.backgroundColor = .white
    self.appBarViewController.navigationController?.navigationBar.backgroundColor = GMColor.imperialblue()
    self.appBarViewController.headerView.backgroundColor = GMColor.imperialblue()
//*************************************************************************//
  }
    

  override func viewDidAppear(_ animated: Bool) {
    super.viewDidAppear(animated)

    if (self.collectionViewLayout is UICollectionViewFlowLayout) {
      let flowLayout = self.collectionViewLayout as! UICollectionViewFlowLayout
        let _: CGFloat = 4.0
      let itemDimension: CGFloat = CGFloat(359)
      let itemSize = CGSize(width: itemDimension, height: CGFloat(200))
      flowLayout.itemSize = itemSize
    }

    if (self.shouldDisplayLogin) {
    //  let loginViewController = LoginViewController(nibName: nil, bundle: nil)
//      self.present(loginViewController, animated: false, completion: nil)
      self.shouldDisplayLogin = false
    }
  }

  //MARK - Methods
  @objc func menuItemTapped(sender: Any) {
 //   let loginViewController = LoginViewController(nibName: nil, bundle: nil)
  //  self.present(loginViewController, animated: true, completion: nil)
    let menu:MenuViewController = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "menu") as! MenuViewController
    let transition = CATransition()
    transition.duration = 0.3
    transition.type = kCATransitionPush
    transition.subtype = kCATransitionFromLeft
    transition.timingFunction = CAMediaTimingFunction(name: kCAMediaTimingFunctionEaseInEaseOut)
    view.window!.layer.add(transition, forKey: kCATransition)
    self.present(menu, animated: false, completion: nil)
    print("Ive been tapped!")
  }
    @objc func bluetoothtapped(sender: Any){
        print("Bluetooth!")
        
        if heartRatePeripheral != nil
        {
            print("Hi 2")
            let alert = UIAlertController(title: "Disconnected from device!", message: nil, preferredStyle: .alert)
            alert.addAction(UIAlertAction(title: "Ok", style: .default, handler: { action in
                print("Yay! You brought your towel!")
            }))
            self.present(alert, animated: true)
            centralManager?.cancelPeripheralConnection(heartRatePeripheral!)
           // centralManager?.connect(heartRatePeripheral)
            heartRatePeripheral = nil
        }
        else{
            self.centralManager.scanForPeripherals(withServices: [hearRateserviceCBUUID])
            /*
            let alert = UIAlertController(title: "do you want to connected to device?", message: nil, preferredStyle: .alert)
            alert.addAction(UIAlertAction(title: "yes", style: .default, handler: { action in
                self.centralManager.scanForPeripherals(withServices: [hearRateserviceCBUUID])
            }))
            alert.addAction(UIAlertAction(title: "no", style: .default, handler: nil))
            self.present(alert, animated: true)
            print("Hiii")
  */
        }

    }

  //MARK - UICollectionViewDataSource
  override func collectionView(_ collectionView: UICollectionView,
                               numberOfItemsInSection section: Int) -> Int {
    //TODO: Set the number of cells to be equal to the number of products in the catalog
    return Catalog.count
  }

    override func collectionView(_ collectionView: UICollectionView,
                               cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
    let cell = self.collectionView?.dequeueReusableCell(withReuseIdentifier: "ProductCell",
                                                        for: indexPath) as! ProductCell
    //TODO: Set the properties of the cell to reflect to product from the model
    let product = Catalog.productAtIndex(index: indexPath.row)
    cell.imageView.image = UIImage(named: product.imageName)!.withRenderingMode(.alwaysTemplate)
    cell.imageView.tintColor = GMColor.imperialblue()
    cell.nameLabel.text = product.productName
    cell.priceLabel.text = product.price
    cell.indexPath = indexPath
    cell.delegate = self
    cell.updateChart()
    return cell
  }


}

//MARK: - UIScrollViewDelegate

// The following four methods must be forwarded to the tracking scroll view in order to implement
// the Flexible Header's behavior.

//TODO: Send the scrollView delegate messages to our appBar's headerView
extension HomeViewController {

  override func scrollViewDidScroll(_ scrollView: UIScrollView) {
    if (scrollView == self.appBarViewController.headerView.trackingScrollView) {
      self.appBarViewController.headerView.trackingScrollDidScroll()
    }
  }

  override func scrollViewDidEndDecelerating(_ scrollView: UIScrollView) {
    if (scrollView == self.appBarViewController.headerView.trackingScrollView) {
      self.appBarViewController.headerView.trackingScrollDidEndDecelerating()
    }
  }

  override func scrollViewDidEndDragging(_ scrollView: UIScrollView,
                                         willDecelerate decelerate: Bool) {
    let headerView = self.appBarViewController.headerView
    if (scrollView == headerView.trackingScrollView) {
      headerView.trackingScrollDidEndDraggingWillDecelerate(decelerate)
    }
  }

  override func scrollViewWillEndDragging(_ scrollView: UIScrollView,
                                          withVelocity velocity: CGPoint,
                                          targetContentOffset: UnsafeMutablePointer<CGPoint>) {
    let headerView = self.appBarViewController.headerView
    if (scrollView == headerView.trackingScrollView) {
      headerView.trackingScrollWillEndDragging(withVelocity: velocity,
                                               targetContentOffset: targetContentOffset)
    }
  }

}
//*********************************BLUETOOTH STUFF*******************************************************\\
extension HomeViewController: CBCentralManagerDelegate{
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        switch central.state {
        case .unknown:
            print("central.state is .unknown")
        case .resetting:
            print("central.state is .resetting")
        case .unsupported:
            print("central.state is .unsupported")
        case .unauthorized:
            print("central.state is .unauthorized")
        case .poweredOff:
            print("central.state is .poweredOff")
        case .poweredOn:
            print("central.state is .poweredOn")
            
        }
        
}
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        print(peripheral.name!)
        if peripherals.contains(peripheral){
        }
        else{
        peripherals.append(peripheral)
        }
        /*
        heartRatePeripheral = peripheral
        heartRatePeripheral.delegate = self
        centralManager.stopScan()// stop scanning once HRM is found
        centralManager.connect(heartRatePeripheral)
        let alert = UIAlertController(title: "what device?", message: nil, preferredStyle: .alert)
        alert.addAction(UIAlertAction(title: "no", style: .default, handler: nil))
        self.present(alert, animated: true)
         */     let alert = UIAlertController(title: "Connect to a device:", message: nil, preferredStyle: .alert)
        for p in peripherals{
            print(p.name!)
            alert.addAction(UIAlertAction(title: p.name, style: .default, handler: { action in
                self.heartRatePeripheral = peripheral
                self.heartRatePeripheral.delegate = self
                self.centralManager.stopScan()// stop scanning once HRM is found
                self.centralManager.connect(self.heartRatePeripheral)
            }))
        }
        alert.addAction(UIAlertAction(title: "Search again", style: .default, handler: { action in
            self.centralManager.scanForPeripherals(withServices: [hearRateserviceCBUUID])
        }))
        alert.addAction(UIAlertAction(title: "Cancel", style: .default, handler: nil))
        
        self.present(alert, animated: true)
        
    }
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        print("Connected!")
        peripherals.removeAll()
        let alert = UIAlertController(title: "Connected to device!", message: nil, preferredStyle: .alert)
        alert.addAction(UIAlertAction(title: "Ok", style: .default, handler: nil))
        
        self.present(alert, animated: true)
        heartRatePeripheral.discoverServices([hearRateserviceCBUUID])
        print("Heart Rate Discovered!")
        heartRatePeripheral.discoverServices([batteryServiceCBUUID])
        print("Bettery Service Discovered!")
        heartRatePeripheral.discoverServices([healthThermometerCBUUID])
        print("Health Thermometer Discovered!")
        heartRatePeripheral.discoverServices([P24ServiceCBUUID])
        print("P24 Service discovered!")
        //posibly discover other services here?
    }

    
    
}
extension HomeViewController: CBPeripheralDelegate{
    //dicover services
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        guard let services = peripheral.services else { return }
        //discover characteristics
        for service in services {
            print(service)
            peripheral.discoverCharacteristics(nil, for: service)
        }
        
}
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService,
                    error: Error?) {
        guard let characteristics = service.characteristics else { return }
        
        for characteristic in characteristics {
            print(characteristic)
            if characteristic.properties.contains(.read) {
                print("\(characteristic.uuid): properties contains .read")
                peripheral.readValue(for: characteristic)
                
            }
            if characteristic.properties.contains(.notify) {
                print("\(characteristic.uuid): properties contains .notify")
                peripheral.setNotifyValue(true, for: characteristic)
            }
            if characteristic.properties.contains(.indicate){
                print("\(characteristic.uuid): properties contains .indicate")
                peripheral.setNotifyValue(true, for: characteristic)
            }
            
        }
    }
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic,
                    error: Error?) {
        switch characteristic.uuid {
        case bodySensorLocationCharacteristicCBUUID:
            let bodySensorLocation = bodyLocation(from: characteristic)
           print(bodySensorLocation)
            
        case heartRateMeasurementCharacteristicCBUUID:
            let bpm = heartRate(from: characteristic)
            print(bpm)
            Catalog.heartRateval = String(bpm)
            let y = String(bpm) //+ "BPM"
            let index = IndexPath(item: 0, section: 0)
            
            

            if self.collectionView?.cellForItem(at: index) != nil{
            let cell = self.collectionView?.cellForItem(at: index) as! ProductCell
              cell.priceLabel.text = String(y)
              cell.unit.isHidden = false
              cell.unit.text = "BPM"
              cell.statuspoint.isHidden = false
              cell.linechart.isHidden = false
              cell.line.label = "BPM"
              
                if (151 >= bpm) && (bpm > 150) {
                    cell.statuspoint.textColor = .red
                    delegate?.send(str:"Heart Rate", str2: "Heart Rate above 150")
                }
                cell.visitors.append(Double(y) as! Double)
              cell.updateChart()
            }
            //add more characteristics implementations
            
        case batteryLevelCBUUID:
            let characteristicData = characteristic.value
            //print(characteristicData!.first!)
            let batterylevel = characteristicData!.first!
            print(batterylevel)
            let index = IndexPath(item: 2, section: 0)
            if self.collectionView?.cellForItem(at: index) != nil{
                let cell = self.collectionView?.cellForItem(at: index) as! ProductCell
                let lvalue = String(batterylevel)// + "%"
                cell.priceLabel.text = lvalue//Update value
                cell.unit.isHidden = false
                cell.unit.text = "%"
                cell.statuspoint.isHidden = false
                cell.linechart.isHidden = false
                cell.visitors.append(Double(batterylevel))
                cell.updateChart()
            }
            
        case tempMeasuermentCBUUID:
            
            let data = characteristic.value
            var array = UnsafeMutablePointer<UInt8>(OpaquePointer(((data as NSData?)?.bytes)!))
            let flags = NORCharacteristicReader.readUInt8Value(ptr: &array)
            let tempInFahrenheit : Bool = (flags & 0x01) > 0
            let _ : Bool = (flags & 0x02) > 0
            let _      : Bool = (flags & 0x04) > 0
            var tempValue        : Float = NORCharacteristicReader.readFloatValue(ptr: &array)
            if tempInFahrenheit == true {
                tempValue = (tempValue - 32.0) * 5.0 / 9.0
            }
            
            print(tempValue)
            let index = IndexPath(item: 1, section: 0)
            if self.collectionView?.cellForItem(at: index) != nil{
                let cell = self.collectionView?.cellForItem(at: index) as! ProductCell
                let lvalue = String(tempValue)
                cell.priceLabel.text = lvalue //Update value
                cell.unit.isHidden = false
                cell.unit.text = "°C"
                cell.statuspoint.isHidden = false
                cell.linechart.isHidden = false
                cell.visitors.append(Double(tempValue))
                cell.updateChart()
            }
            
        case stepCountCharacteristicCBUUID:
             let characteristicData = characteristic.value
             let byteArray = [UInt8](characteristicData!)
             let stepcountint = (Int(byteArray[0]) << 24) + (Int(byteArray[1]) << 16) + (Int(byteArray[2]) << 8) + Int(byteArray[3])
             let stepcount = String(stepcountint)
            print("test value is \(stepcount)" )
             let index = IndexPath(item: 4, section: 0)
             if self.collectionView?.cellForItem(at: index) != nil{
                let cell = self.collectionView?.cellForItem(at: index) as! ProductCell
                cell.priceLabel.text = String(stepcount) //Update value
            //    cell.unit.isHidden = false
                cell.unit.text = ""
                cell.linechart.isHidden = false
                cell.statuspoint.isHidden = false
                cell.visitors.append(Double(stepcount) as! Double)
                cell.updateChart()
            }
        case bloodOxygenCharacteristicCBUUID:
            let characteristicData = characteristic.value
            let byteArray = [UInt8](characteristicData!)
            let bloodoxint = (Int(byteArray[0]) << 24) + (Int(byteArray[1]) << 16) + (Int(byteArray[2]) << 8) + Int(byteArray[3])
            let bloodox = String(bloodoxint)
            print("test value 2 is \(bloodox)" )
            let index = IndexPath(item: 5, section: 0)
            if self.collectionView?.cellForItem(at: index) != nil{
                let cell = self.collectionView?.cellForItem(at: index) as! ProductCell
                cell.priceLabel.text = String(bloodox) //Update value
                cell.unit.isHidden = false
                cell.unit.text = "%"
                cell.statuspoint.isHidden = false
                cell.linechart.isHidden = false
                cell.visitors.append(Double(bloodox) as! Double)
                cell.updateChart()
            }
       
        default:
            print("Unhandled Characteristic UUID: \(characteristic.uuid)")
            
        }
    }
    
    
    private func bodyLocation(from characteristic: CBCharacteristic) -> String {
        guard let characteristicData = characteristic.value,
            let byte = characteristicData.first else { return "Error" }
        
        switch byte {
        case 0: return "Other"
        case 1: return "Chest"
        case 2: return "Wrist"
        case 3: return "Finger"
        case 4: return "Hand"
        case 5: return "Ear Lobe"
        case 6: return "Foot"
        default:
            return "Reserved for future use"
        }
    }
    
    //heart rate values interpretation(see tutorial for more info):
    private func heartRate(from characteristic: CBCharacteristic) -> Int {
        guard let characteristicData = characteristic.value else { return -1 }
        let byteArray = [UInt8](characteristicData)
        
        let firstBitValue = byteArray[0] & 0x01
        if firstBitValue == 0 {
            // Heart Rate Value Format is in the 2nd byte
            return Int(byteArray[1])
        } else {
            // Heart Rate Value Format is in the 2nd and 3rd bytes
            return (Int(byteArray[1]) << 8) + Int(byteArray[2])
        }
    }
    


}

extension HomeViewController:UICollectionViewDelegateFlowLayout{
    func collectionView(_ collectionView: UICollectionView, layout collectionViewLayout: UICollectionViewLayout, sizeForItemAt indexPath: IndexPath) -> CGSize {
        
        if isExpanded[indexPath.row] == true{
            return CGSize(width: cellWidth, height: expandedHeight)
        }else{
            return CGSize(width: cellWidth, height: notExpandedHeight)
        }
        
    }
    
}

extension HomeViewController:ExpandedCellDelegate{
    func topButtonTouched(indexPath: IndexPath) {
        print("I know!")
        isExpanded[indexPath.row] = !isExpanded[indexPath.row]
        UIView.animate(withDuration: 0.8, delay: 0.0, usingSpringWithDamping: 0.9, initialSpringVelocity: 0.9, options: UIViewAnimationOptions.curveEaseInOut, animations: {
            self.collectionView!.reloadItems(at: [indexPath])
        }, completion: { success in
            print("success")
        })
}
}

