//
//  alerHistoryR.swift
//  Shrine
//
//  Created by Zaid Aggad on 10/06/2019.
//  Copyright © 2019 Google. All rights reserved.
//

import UIKit
import MaterialComponents


class alerHistoryR: UIViewController, UITableViewDataSource, UITableViewDelegate, SendManager {
    func send(str: String, str2: String) {
        self.alertslist.append(str)
        self.info.append(str2)
        self.dates.append(printDate())
        let index2 = IndexPath(row: (self.alertslist.count)-1, section: 0 )
        if self.alerts != nil{
        self.alerts.beginUpdates()
        self.alerts.insertRows(at: [index2], with: .automatic)
        self.alerts.endUpdates()
        for a in alertslist{
            print(a)
        }
        }
    }
    
    public var alertslist = ["Heart Rate", "Skin Temperature", "Blood Oxygen"]
    public var dates = ["21/05/19 at 13:23", "23/05/19 at 12:20", "24/05/19 at 05:49"]
    public var info = ["Heart rate above 150", "Skin temperature abobe 37", "Blood Oxygen below 90%"]
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return alertslist.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "alerttcell", for: indexPath) as! alertcell
        cell.title.text = alertslist[indexPath.row]
        cell.title.textColor = GMColor.imperialorange()
        cell.date.text = dates[indexPath.row]
        cell.information.text = info[indexPath.row]
        cell.Aimage.tintColor =  GMColor.imperialorange()
        return cell
    }
    
    var appBarViewController = MDCAppBarViewController()
    
    @IBOutlet weak var alerts: UITableView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.view.backgroundColor = .white
        self.appBarViewController.navigationController?.navigationBar.backgroundColor = GMColor.imperialblue()
        self.appBarViewController.headerView.backgroundColor = GMColor.imperialblue()
        self.appBarViewController.navigationController?.navigationBar.backgroundColor = GMColor.imperialblue()
        self.view.tintColor = .white
        let titleDict: NSDictionary = [NSAttributedStringKey.foregroundColor: UIColor.white]
        self.appBarViewController.navigationBar.titleTextAttributes = (titleDict as! [NSAttributedStringKey : Any])
        self.view.tintColor = .white
        self.title = "Alert History"
        self.addChildViewController(self.appBarViewController)
        self.view.addSubview(self.appBarViewController.view)
        alerts.tintColor = .red
        self.appBarViewController.didMove(toParentViewController: self)
        
        let menuItemImage = UIImage(named: "MenuItem")
        let templatedMenuItemImage = menuItemImage?.withRenderingMode(.alwaysTemplate)
        let menuItem = UIBarButtonItem(image: templatedMenuItemImage,
                                       style: .plain,
                                       target: self,
                                       action: #selector(menuItemTapped(sender:)))
        self.navigationItem.leftBarButtonItem = menuItem
        // Do any additional setup after loading the view.
    }
    
    @objc func menuItemTapped(sender: Any) {
        let transition = CATransition()
        transition.duration = 0.3
        transition.type = kCATransitionPush
        transition.subtype = kCATransitionFromLeft
        transition.timingFunction = CAMediaTimingFunction(name: kCAMediaTimingFunctionEaseInEaseOut)
        view.window!.layer.add(transition, forKey: kCATransition)
        self.dismiss(animated: false, completion: nil)
    }
    func printDate() -> String {
        let date = Date()
        let formatter1 = DateFormatter()
        let formatter2 = DateFormatter()
        formatter1.dateFormat = "dd.MM.yyyy"
        formatter2.dateFormat = "HH:mm:ss"
        let result = formatter1.string(from: date) + " at " + formatter2.string(from: date)
        
        return result
    }
    

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destination.
        // Pass the selected object to the new view controller.
    }
    */

}
